// Fill out your copyright notice in the Description page of Project Settings.

#include "SwordGameServerConnectionSubsystem.h"

#include "Engine/World.h"
#include "SwordGameGameUserSettings.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Blueprint/UserWidget.h"
#include "SwordGameServerConnectionWidgetInterface.h"
#include "Engine/NetworkDelegates.h"
#include "GameDelegates.h"
#include "Blueprint/GameViewportSubsystem.h"

DEFINE_LOG_CATEGORY_STATIC(LogSwordGameServerConnectionSubsystem, All, Log);

namespace
{
    float SwordGame_ServerConnection_AttemptTimeoutSeconds = 8.f;

    FAutoConsoleVariableRef CVar_SwordGame_ServerConnection_AttemptTimeoutSeconds(
        TEXT("SwordGame.ServerConnection.AttemptTimeoutSeconds"),
        SwordGame_ServerConnection_AttemptTimeoutSeconds,
        TEXT("Amount of time to wait, in seconds, before giving up / retrying to connect.")
    );

    bool SwordGame_Test_ServerConnection_AlwaysTimeout = false;

    FAutoConsoleVariableRef CVar_SwordGame_Test_ServerConnection_AlwaysTimeout(
        TEXT("SwordGame.Test.ServerConnection.AlwaysTimeout"),
        SwordGame_Test_ServerConnection_AlwaysTimeout,
        TEXT("If true, simulate always failing (timing out) to connect.")
    );
}

namespace
{
    bool SwordGame_NetworkVersion_ShouldServerAcceptIncompatibleVersions = false;

    FAutoConsoleVariableRef CVar_SwordGame_NetworkVersion_ShouldServerAcceptIncompatibleVersions(
        TEXT("SwordGame.NetworkVersion.ShouldServerAcceptIncompatibleVersions"),
        SwordGame_NetworkVersion_ShouldServerAcceptIncompatibleVersions,
        TEXT("If true, the server will allow clients with differing network versions to join.")
    );

    int32 SwordGame_NetworkVersion_LocalOverride = -1;

    FAutoConsoleVariableRef CVar_SwordGame_NetworkVersion_LocalOverride(
        TEXT("SwordGame.NetworkVersion.LocalOverride"),
        SwordGame_NetworkVersion_LocalOverride,
        TEXT("If non-negative, override the local network checksum value. When connecting to a server, this will be sent when connecting. When hosting a server, this will be used to compare against the versions of new clients who are connecting. Set to `-1` to disable.")
    );

    FDelayedAutoRegisterHelper TowersStartEnginePreInitDelayedAutoRegisterHelper(
        EDelayedRegisterRunPhase::StartOfEnginePreInit,
        []() -> void
        {
            ensure(FNetworkVersion::IsNetworkCompatibleOverride.IsBound() == false);
            FNetworkVersion::IsNetworkCompatibleOverride.BindStatic(
                [](uint32 localNetworkVersion, uint32 remoteNetworkVersion) -> bool
                {
                    if (SwordGame_NetworkVersion_ShouldServerAcceptIncompatibleVersions)
                    {
                        return true;
                    }

                    // No override specified. Proceed to the default behavior.

                    // Unbind our override, call the original implementation, and restore our override afterward.
                    const TGuardValue scopedGuardDelegate(
                        FNetworkVersion::IsNetworkCompatibleOverride,
                        FNetworkVersion::FIsNetworkCompatibleOverride());

                    return FNetworkVersion::IsNetworkCompatible(localNetworkVersion, remoteNetworkVersion);
                });

            ensure(FNetworkVersion::GetLocalNetworkVersionOverride.IsBound() == false);
            FNetworkVersion::GetLocalNetworkVersionOverride.BindStatic(
                []() -> uint32
                {
                    if (SwordGame_NetworkVersion_LocalOverride >= 0)
                    {
                        return static_cast<uint32>(SwordGame_NetworkVersion_LocalOverride);
                    }

                    // No override specified. Proceed to the default behavior.
                    constexpr bool shouldAllowOverrideDelegate = false;
                    return FNetworkVersion::GetLocalNetworkVersion(shouldAllowOverrideDelegate);
                });
        }
    );
}

#define LOCTEXT_NAMESPACE "SwordGameServerConnectionSubsystem"

FText FConnectionSequenceArgs::GetDefaultConnectingToServerStatusText()
{
    return LOCTEXT("ServerConnectionWidget_Status_GetDefaultConnectingToServerStatusText", "Connecting to server...");
}

FText FConnectionSequenceArgs::GetDefaultFailedToConnectFormattedStatusText()
{
    return LOCTEXT("ServerConnectionWidget_Status_GetDefaultFailedToConnectFormattedStatusText", "Failed to connect after {0} attempts");
}

void USwordGameServerConnectionSubsystem::Initialize(FSubsystemCollectionBase& collection)
{
    Super::Initialize(collection);

    OnStartGameInstanceDelegateHandle = FWorldDelegates::OnStartGameInstance.AddLambda(
        [this](UGameInstance* gameInstance)
        {
            check(gameInstance);
            OnStartGameInstance(*gameInstance);
        }
    );

    check(GEngine);
    OnTravelFailureDelegateHandle = GEngine->OnTravelFailure().AddLambda(
        [this](UWorld* world, ETravelFailure::Type failureType, const FString& errorString)
        {
            OnTravelFailure(world, failureType, errorString);
        }
    );

    check(GEngine);
    OnNetworkFailureDelegateHandle = GEngine->OnNetworkFailure().AddLambda(
        [this](UWorld* world, UNetDriver* netDriver, ENetworkFailure::Type failureType, const FString& errorString)
        {
            OnNetworkFailure(world, netDriver, failureType, errorString);
        }
    );

    HandleDisconnectDelegateHandle = FGameDelegates::Get().GetHandleDisconnectDelegate().AddLambda(
        [this](UWorld* inWorld, UNetDriver* netDriver)
        {
            OnHandleDisconnect(inWorld, netDriver);
        }
    );

    OnPendingNetGameConnectionCreatedDelegateHandle = FNetDelegates::OnPendingNetGameConnectionCreated.AddLambda(
        [this](UPendingNetGame* pendingNetGame)
        {
            check(pendingNetGame);
            OnPendingNetGameConnectionCreated(*pendingNetGame);
        }
    );

    PreLoadMapWithContextDelegateHandle = FCoreUObjectDelegates::PreLoadMapWithContext.AddLambda(
        [this](const FWorldContext& worldContext, const FString& mapName)
        {
            OnPreLoadMap(worldContext, mapName);
        }
    );

    PostLoadMapWithWorldDelegateHandle = FCoreUObjectDelegates::PostLoadMapWithWorld.AddLambda(
        [this](UWorld* loadedWorld)
        {
            OnPostLoadMap(loadedWorld);
        }
    );
}

void USwordGameServerConnectionSubsystem::Deinitialize()
{
    GetGameInstanceChecked().GetTimerManager().ClearTimer(ConnectionAttemptTimerHandle);

    FWorldDelegates::OnStartGameInstance.Remove(OnStartGameInstanceDelegateHandle);

    check(GEngine);
    GEngine->OnTravelFailure().Remove(OnTravelFailureDelegateHandle);
    GEngine->OnNetworkFailure().Remove(OnNetworkFailureDelegateHandle);

    FGameDelegates::Get().GetHandleDisconnectDelegate().Remove(HandleDisconnectDelegateHandle);

    FNetDelegates::OnPendingNetGameConnectionCreated.Remove(OnPendingNetGameConnectionCreatedDelegateHandle);
    FCoreUObjectDelegates::PreLoadMapWithContext.Remove(PreLoadMapWithContextDelegateHandle);
    FCoreUObjectDelegates::PostLoadMapWithWorld.Remove(PostLoadMapWithWorldDelegateHandle);

    Super::Deinitialize();
}

void USwordGameServerConnectionSubsystem::KickOffAutoConnectSequence()
{
    const USwordGameGameUserSettings& myGameUserSettings = USwordGameGameUserSettings::GetChecked();
    ensureMsgf(myGameUserSettings.GetIsAutoConnectEnabled(), TEXT("This should not have been called."));

    constexpr ETravelType travelType = ETravelType::TRAVEL_Absolute;

    KickOffConnectionSequence(
        FConnectionSequenceArgs(
            FString(myGameUserSettings.GetAutoConnectURL()),
            travelType
        ).SetNumTimesToTry(myGameUserSettings.GetAutoConnectNumTries())
    );
}

void USwordGameServerConnectionSubsystem::TryKickOffAutoReconnectSequence(const FURL& lastRemoteURL)
{
    const USwordGameGameUserSettings& myGameUserSettings = USwordGameGameUserSettings::GetChecked();
    check(myGameUserSettings.GetIsAutoReconnectEnabled());

    if (NumTimesAutoReconnected >= myGameUserSettings.GetAutoReconnectMaxTimes())
    {
        UE_LOG(LogSwordGameServerConnectionSubsystem, Warning, TEXT("%s"),
            TStringBuilder<512>(EInPlace::InPlace,
                TEXT("Exceeded the max number of times to reconnect: "), NumTimesAutoReconnected, TEXT('/'), myGameUserSettings.GetAutoReconnectMaxTimes(), TEXT(".")
                TEXT(" ")
                TEXT("We're not kicking off anymore auto-reconnects.")
            ).ToString()
        );
        return;
    }

    KickOffAutoReconnectSequence(lastRemoteURL);
}

void USwordGameServerConnectionSubsystem::KickOffAutoReconnectSequence(const FURL& lastRemoteURL)
{
    const USwordGameGameUserSettings& myGameUserSettings = USwordGameGameUserSettings::GetChecked();
    check(myGameUserSettings.GetIsAutoReconnectEnabled());
    check(NumTimesAutoReconnected < myGameUserSettings.GetAutoReconnectMaxTimes());

    ++NumTimesAutoReconnected;

    // A reconnect implies that we've already had some success with connecting in the first place, so halve the
    // number of tries on this connection sequence because they are likely unnecessary.
    const uint8 numTimesToTry = myGameUserSettings.GetAutoConnectNumTries() / 2;

    KickOffConnectionSequence(
        FConnectionSequenceArgs(
            lastRemoteURL
        ).SetNumTimesToTry(numTimesToTry)
        .SetConnectingToServerStatusText(LOCTEXT("ServerConnectionWidget_Status_AutoReconnect", "Reconnecting..."))
        .SetFailedToConnectFormattedStatusText(LOCTEXT("ServerConnectionWidget_Status_AutoReconnectFail", "Failed to reconnect after {0} attempts"))
    );
}

void USwordGameServerConnectionSubsystem::KickOffConnectionSequence(FConnectionSequenceArgs&& args)
{
    const UWorld* world = GetWorld();
    checkf(world, TEXT("This should not have been called at this state. It's expected for the world to be valid."));

    ensureMsgf(!IsCurrentlyInConnectionSequence(), TEXT("This should be the first connection try in the sequence."));
    ensureMsgf(GetCurrentConnectionSequenceTry() == 0u, TEXT("This should be the first connection try in the sequence."));

    CurrentConnectionSequenceConnectingToServerStatusText = MoveTemp(args.ConnectingToServerStatusText);
    CurrentConnectionSequenceFailedToConnectFormattedStatusText = MoveTemp(args.FailedToConnectFormattedStatusText);
    CurrentConnectionSequenceURL = MoveTemp(args.URL);
    CurrentConnectionSequenceTravelType = args.TravelType;
    CurrentConnectionSequenceNumTimesToTry = args.NumTimesToTry;

    check(GEngine);
    CurrentConnectionSequenceLastURL = GEngine->GetWorldContextFromWorldChecked(world).LastURL;

    UE_LOG(LogSwordGameServerConnectionSubsystem, Display, TEXT("%s"),
        TStringBuilder<512>(EInPlace::InPlace,
            TEXT("Kicking off connection sequence.")
            TEXT(" ")
            TEXT("URL: '"), CurrentConnectionSequenceURL, TEXT("'.")
            TEXT(" ")
            TEXT("Number of times to try: "), CurrentConnectionSequenceNumTimesToTry, TEXT(".")
        ).ToString()
    );

    TryConnect();
}

void USwordGameServerConnectionSubsystem::EndCurrentConnectionSequence()
{
    GetGameInstanceChecked().GetTimerManager().ClearTimer(ConnectionAttemptTimerHandle);
    ResetCurrentConnectionSequenceData();
}

uint8 USwordGameServerConnectionSubsystem::GetDefaultConnectionSequenceNumTimesToTry()
{
    return USwordGameGameUserSettings::GetChecked().GetAutoConnectNumTries();
}

FText USwordGameServerConnectionSubsystem::GetConnectedConnectionStatusText()
{
    return LOCTEXT("ServerConnectionWidget_Status_Connected", "Connected");
}

FText USwordGameServerConnectionSubsystem::GetDisconnectedConnectionStatusText()
{
    return LOCTEXT("ServerConnectionWidget_Status_Disconnected", "Disconnected");
}

void USwordGameServerConnectionSubsystem::OnStartGameInstance(UGameInstance& gameInstance)
{
    TRACE_CPUPROFILER_EVENT_SCOPE(USwordGameServerConnectionSubsystem::OnStartGameInstance);

    // By this point, we have just finished browsing to the game default map. See `UGameInstance::StartGameInstance`.

    bool isStandaloneNetMode = true;

    if (const UWorld* world = gameInstance.GetWorld())
    {
        isStandaloneNetMode = world->IsNetMode(ENetMode::NM_Standalone);
    }

    if (isStandaloneNetMode)
    {
        LoadCreateAddServerConnectionUserWidget(gameInstance);

        if (USwordGameGameUserSettings::GetChecked().GetIsAutoConnectEnabled())
        {
            // If we're in singleplayer, try connecting to a server.
            KickOffAutoConnectSequence();
        }
    }
}

void USwordGameServerConnectionSubsystem::LoadCreateAddServerConnectionUserWidget(UGameInstance& gameInstance)
{
    if (ServerConnectionUserWidgetClassSoft.IsNull())
    {
        UE_LOG(LogSwordGameServerConnectionSubsystem, Error, TEXT("%s"),
            TStringBuilder<512>(EInPlace::InPlace,
                TEXT("Failed to load server connection user widget '"), ServerConnectionUserWidgetClassSoft.GetLongPackageFName(), TEXT("'.")
                TEXT(" ")
                TEXT("The asset path is empty.")
            ).ToString()
        );
        return;
    }

    FStreamableAsyncLoadParams asyncLoadParams;
    asyncLoadParams.TargetsToStream.Emplace(ServerConnectionUserWidgetClassSoft.ToSoftObjectPath());

    TSharedPtr<FStreamableHandle> streamableHandleSharedPtr =
        UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(MoveTemp(asyncLoadParams), ServerConnectionUserWidgetClassSoft.GetLongPackageName());

    if (!ensure(streamableHandleSharedPtr.IsValid()))
    {
        UE_LOG(LogSwordGameServerConnectionSubsystem, Error, TEXT("%s"),
            TStringBuilder<512>(EInPlace::InPlace,
                TEXT("Failed to load server connection user widget '"), ServerConnectionUserWidgetClassSoft.GetLongPackageFName(), TEXT("'.")
                TEXT(" ")
                TEXT("Streamable handle was null.")
            ).ToString()
        );
        return;
    }

    FStreamableHandle& streamableHandle = *streamableHandleSharedPtr.Get();

    const EAsyncPackageState::Type asyncPackageState = streamableHandle.WaitUntilComplete();
    if (!ensure(asyncPackageState == EAsyncPackageState::Complete))
    {
        UE_LOG(LogSwordGameServerConnectionSubsystem, Error, TEXT("%s"),
            TStringBuilder<512>(EInPlace::InPlace,
                TEXT("Package state failed to complete while waiting for '"), ServerConnectionUserWidgetClassSoft.GetLongPackageFName(), TEXT("' to finish loading.")
            ).ToString()
        );
    }

    UObject* loadedAsset = streamableHandle.GetLoadedAsset();
    ensure(loadedAsset);
    UClass* loadedClass = Cast<UClass>(loadedAsset);
    ensure(loadedClass);
    TSubclassOf<UUserWidget> loadedWidgetClass = loadedClass;
    if (!ensure(loadedWidgetClass))
    {
        UE_LOG(LogSwordGameServerConnectionSubsystem, Error, TEXT("%s"),
            TStringBuilder<512>(EInPlace::InPlace,
                TEXT("The loaded widget class is null.")
                TEXT(" ")
                TEXT("Loaded asset: '"), GetFNameSafe(loadedAsset), TEXT("'.")
            ).ToString()
        );
        return;
    }

    if (!loadedWidgetClass->ImplementsInterface(ISwordGameServerConnectionWidgetInterface::UClassType::StaticClass()))
    {
        UE_LOG(LogSwordGameServerConnectionSubsystem, Error, TEXT("%s"),
            TStringBuilder<512>(EInPlace::InPlace,
                TEXT("The loaded widget class '"), loadedWidgetClass->GetFName(), TEXT("' does not implement'"), ISwordGameServerConnectionWidgetInterface::UClassType::StaticClass()->GetFName(), TEXT("'.")
            ).ToString()
        );
        return;
    }

    UUserWidget* createdWidget = UUserWidget::CreateWidgetInstance(gameInstance, loadedWidgetClass, loadedWidgetClass->GetFName());
    if (!ensure(createdWidget))
    {
        UE_LOG(LogSwordGameServerConnectionSubsystem, Error, TEXT("%s"),
            TStringBuilder<512>(EInPlace::InPlace,
                TEXT("Failed to create widget.")
                TEXT(" ")
                TEXT("Widget class: '"), loadedWidgetClass->GetFName(), TEXT("'.")
            ).ToString()
        );
        return;
    }

    createdWidget->AddToViewport();

    // Prevent this widget from being removed across world travels.
    {
        UGameViewportSubsystem* gameViewportSubsystem = UGameViewportSubsystem::Get();
        check(gameViewportSubsystem);

        FGameViewportWidgetSlot widgetSlot = gameViewportSubsystem->GetWidgetSlot(createdWidget);
        widgetSlot.bAutoRemoveOnWorldRemoved = false;
        gameViewportSubsystem->SetWidgetSlot(createdWidget, MoveTemp(widgetSlot));
    }

    ISwordGameServerConnectionWidgetInterface* createdWidgetCasted = Cast<ISwordGameServerConnectionWidgetInterface>(createdWidget);
    check(createdWidgetCasted);

    // Set both the object and interface, just in case, because I've seen bugs with this engine type before.
    ServerConnectionUserWidgetInstance.SetObject(createdWidget);
    ServerConnectionUserWidgetInstance.SetInterface(createdWidgetCasted);
}

void USwordGameServerConnectionSubsystem::TryConnect()
{
    UWorld* world = GetWorld();
    checkf(world, TEXT("This should not have been called at this state. It's expected for the world to be valid."));

    ensure(CurrentConnectionSequenceTry < CurrentConnectionSequenceNumTimesToTry);

    ++CurrentConnectionSequenceTry;

    if (ISwordGameServerConnectionWidgetInterface* serverConnectionWidget = GetServerConnectionUserWidgetInstance())
    {
        if (CurrentConnectionSequenceTry <= 1u)
        {
            serverConnectionWidget->SetStatus(FText(CurrentConnectionSequenceConnectingToServerStatusText));
        }
        else
        {
            serverConnectionWidget->SetStatus(
                FText::Format(
                    LOCTEXT("ServerConnectionWidget_Status_KickOffConnectionSequence_NextTries", "{0}\n(attempt: {1}/{2})"),
                    CurrentConnectionSequenceConnectingToServerStatusText,
                    CurrentConnectionSequenceTry,
                    CurrentConnectionSequenceNumTimesToTry
                )
            );
        }

        serverConnectionWidget->RetriggerStatusAttention(6.f);
    }

    if (!SwordGame_Test_ServerConnection_AlwaysTimeout)
    {
        // See `UEngine::HandleOpenCommand` for reference on client traveling.

        const TCHAR* nextURL = CurrentConnectionSequenceURL.GetCharArray().GetData();
        const ETravelType travelType = CurrentConnectionSequenceTravelType;

        check(GEngine);
        GEngine->SetClientTravel(world, nextURL, travelType);
    }

    constexpr bool shouldLoop = false;
    GetGameInstanceChecked().GetTimerManager().SetTimer(
        ConnectionAttemptTimerHandle,
        [this]()
        {
            OnConnectionAttemptTimeout();
        },
        SwordGame_ServerConnection_AttemptTimeoutSeconds,
        shouldLoop);
}

void USwordGameServerConnectionSubsystem::TryRetryConnect()
{
    if (CurrentConnectionSequenceTry >= CurrentConnectionSequenceNumTimesToTry)
    {
        UE_LOG(LogSwordGameServerConnectionSubsystem, Warning, TEXT("%s"),
            TStringBuilder<512>(EInPlace::InPlace,
                TEXT("Exceeded the number of time to try connecting: "), CurrentConnectionSequenceTry, TEXT('/'), CurrentConnectionSequenceNumTimesToTry, TEXT(".")
                TEXT(" ")
                TEXT("Ending the current connection sequence on a fail.")
            ).ToString()
        );

        if (ISwordGameServerConnectionWidgetInterface* serverConnectionWidget = GetServerConnectionUserWidgetInstance())
        {
            serverConnectionWidget->SetStatus(
                FText::Format(
                    CurrentConnectionSequenceFailedToConnectFormattedStatusText,
                    CurrentConnectionSequenceTry
                )
            );

            serverConnectionWidget->RetriggerStatusAttention(4.f);
        }

        EndCurrentConnectionSequence();
        return;
    }

    UE_LOG(LogSwordGameServerConnectionSubsystem, Warning, TEXT("%s"),
        TStringBuilder<512>(EInPlace::InPlace,
            TEXT("Retrying connection attempt.")
        ).ToString()
    );

    TryConnect();
}

void USwordGameServerConnectionSubsystem::OnConnectionFailureDuringConnectionSequence()
{
    UE_LOG(LogSwordGameServerConnectionSubsystem, Warning, TEXT("%s"),
        TStringBuilder<512>(EInPlace::InPlace,
            TEXT("Failed to connect to '"), CurrentConnectionSequenceURL, TEXT("' on connection sequence try: "), CurrentConnectionSequenceTry, TEXT(".")
        ).ToString()
    );

    TryRetryConnect();
}

void USwordGameServerConnectionSubsystem::OnTravelFailure(UWorld* world, const ETravelFailure::Type failureType, const FStringView errorString)
{
    if (IsCurrentlyInConnectionSequence())
    {
        // This callback is for our connection sequence code.
        OnTravelFailureDuringOurConnectionSequence(world, failureType, errorString);
    }
}

void USwordGameServerConnectionSubsystem::OnNetworkFailure(UWorld* world, UNetDriver* netDriver, const ENetworkFailure::Type failureType, const FStringView errorString)
{
    if (IsCurrentlyInConnectionSequence())
    {
        // This callback is for our connection sequence code.
        OnNetworkFailureDuringOurConnectionSequence(world, netDriver, failureType, errorString);
    }
}

void USwordGameServerConnectionSubsystem::OnHandleDisconnect(UWorld* inWorld, UNetDriver* netDriver)
{
    if (IsCurrentlyInConnectionSequence())
    {
        return;
    }

    if (ISwordGameServerConnectionWidgetInterface* serverConnectionWidget = GetServerConnectionUserWidgetInstance())
    {
        serverConnectionWidget->SetStatus(GetDisconnectedConnectionStatusText());
        serverConnectionWidget->RetriggerStatusAttention(2.f);
    }

    //
    // @Christian: Note: This gets broadcasted from two possible places:
    // - `UEngine::HandleNetworkFailure`
    //     - Only for certain `ENetworkFailure` values
    //         - `ConnectionLost`
    //         - `ConnectionTimeout`
    //         - `NetGuidMismatch`
    //         - `NetChecksumMismatch`
    // - `UEngine::HandleTravelFailure`
    //     - For any `ETravelFailure` value
    //

    // @Christian: After being disconnected, the engine schedules us to travel to the default map. We set this bool to true so
    // we can keep track of whether a `OnPostLoadMap` event came from a disconnect or not. See: `OnPostLoadMapAfterDisconnect`. We
    // want to make sure our auto-reconnect code happens AFTER the default map load.
    bIsCurrentlyDisconnectedAndScheduledToReturnToDefaultMap = true;
}

void USwordGameServerConnectionSubsystem::OnPendingNetGameConnectionCreated(UPendingNetGame& pendingNetGame)
{
    if (IsCurrentlyInConnectionSequence())
    {
        // This callback is for our connection sequence code.
        OnPendingNetGameConnectionCreatedDuringOurConnectionSequence(pendingNetGame);
    }
}

void USwordGameServerConnectionSubsystem::OnPreLoadMap(const FWorldContext& worldContext, const FStringView mapName)
{
    if (IsCurrentlyInConnectionSequence())
    {
        // This callback is for our connection sequence code.
        OnPreLoadMapDuringOurConnectionSequence(worldContext, mapName);
    }
}

void USwordGameServerConnectionSubsystem::OnPostLoadMap(UWorld* loadedWorld)
{
    if (bIsCurrentlyDisconnectedAndScheduledToReturnToDefaultMap)
    {
        bIsCurrentlyDisconnectedAndScheduledToReturnToDefaultMap = false;
        OnPostLoadMapAfterDisconnect(loadedWorld);
    }
}

void USwordGameServerConnectionSubsystem::OnPostLoadMapAfterDisconnect(UWorld* loadedWorld)
{
    const USwordGameGameUserSettings& myGameUserSettings = USwordGameGameUserSettings::GetChecked();
    if (myGameUserSettings.GetIsAutoReconnectEnabled())
    {
        if (loadedWorld)
        {
            check(GEngine);
            const FWorldContext& loadedWorldContext = GEngine->GetWorldContextFromWorldChecked(loadedWorld);

            // Reconnect to the most recent server URL, i.e., the one we were disconnected from.
            TryKickOffAutoReconnectSequence(loadedWorldContext.LastRemoteURL);
        }
    }
}

void USwordGameServerConnectionSubsystem::OnTravelFailureDuringOurConnectionSequence(
    UWorld* world,
    const ETravelFailure::Type failureType,
    const FStringView& errorString)
{
    OnConnectionFailureDuringConnectionSequence();
}

void USwordGameServerConnectionSubsystem::OnNetworkFailureDuringOurConnectionSequence(
    UWorld* world,
    UNetDriver* netDriver,
    const ENetworkFailure::Type failureType,
    const FStringView& errorString)
{
    OnConnectionFailureDuringConnectionSequence();
}

void USwordGameServerConnectionSubsystem::OnPendingNetGameConnectionCreatedDuringOurConnectionSequence(
    UPendingNetGame& pendingNetGame)
{
    // Pending net game was created for our connection sequence.
}

void USwordGameServerConnectionSubsystem::OnPreLoadMapDuringOurConnectionSequence(
    const FWorldContext& worldContext,
    const FStringView& mapName)
{
    {
        // Ignore the URL options when comparing equality here, because you can't rely on URL options staying the
        // same. @Christian: A direct example of why we do this is the fact that `UEngine::Browse` removes the "closed" option
        // right after calling `UEngine::LoadMap`, which would make the world context's URL differ from the one we store from
        // the auto-reconnect.
        const bool isWorldURLEqualToOurCurrentConnectionSequenceURL = AreURLsEqualDisregardingOps(worldContext.LastURL, CurrentConnectionSequenceLastURL);
        if (!ensure(isWorldURLEqualToOurCurrentConnectionSequenceURL))
        {
            UE_LOG(LogSwordGameServerConnectionSubsystem, Warning, TEXT("%s"),
                TStringBuilder<512>(EInPlace::InPlace,
                    TEXT("The last URL has somehow changed during our connection sequence (maybe caused by an interfering world travel?). Canceling the current connection sequence.")
                ).ToString()
            );

            EndCurrentConnectionSequence();
            return;
        }
    }

    const UPendingNetGame* pendingNetGame = worldContext.PendingNetGame;
    if (!pendingNetGame)
    {
        OnConnectionFailureDuringConnectionSequence();
        return;
    }

    if (!pendingNetGame->bSuccessfullyConnected)
    {
        OnConnectionFailureDuringConnectionSequence();
        return;
    }

#if DO_ENSURE
    if (ensureMsgf(pendingNetGame->NetDriver, TEXT("If pending net game is successfully connected, we expect to have a valid net driver.")))
    {
        ensureMsgf(pendingNetGame->NetDriver->ServerConnection, TEXT("If pending net game is successfully connected, we expect to have a valid server connection."));
    }
#endif // #if DO_ENSURE

    ensureMsgf(pendingNetGame->URL.Host == GetCurrentConnectionSequenceURL().Host, TEXT("Everything seems successful, so we expect the host that we're connected to to match the host which this connection sequence is trying to connect to."));

    // Once we've successfully connected and started to load the map, consider this a success.
    OnConnectionSequenceSuccess();
}

void USwordGameServerConnectionSubsystem::OnConnectionSequenceSuccess()
{
    if (ISwordGameServerConnectionWidgetInterface* serverConnectionWidget = GetServerConnectionUserWidgetInstance())
    {
        serverConnectionWidget->SetStatus(GetConnectedConnectionStatusText());
        serverConnectionWidget->RetriggerStatusAttention(1.f);
    }

    EndCurrentConnectionSequence();
}

void USwordGameServerConnectionSubsystem::OnConnectionAttemptTimeout()
{
    check(GEngine);
    GEngine->CancelAllPending();

    OnConnectionFailureDuringConnectionSequence();
}

void USwordGameServerConnectionSubsystem::ResetCurrentConnectionSequenceData()
{
    CurrentConnectionSequenceLastURL = FURL();
    CurrentConnectionSequenceConnectingToServerStatusText = FText();
    CurrentConnectionSequenceFailedToConnectFormattedStatusText = FText();
    CurrentConnectionSequenceURL = FString();
    CurrentConnectionSequenceTravelType = static_cast<ETravelType>(0u);
    CurrentConnectionSequenceNumTimesToTry = 0u;
    CurrentConnectionSequenceTry = 0u;
}

bool USwordGameServerConnectionSubsystem::AreURLsEqualDisregardingOps(const FURL& a, const FURL& b)
{
    // Empty their `Op` values (but save them for later).
    TArray<FString> aOp = MoveTemp(const_cast<FURL&>(a).Op);
    TArray<FString> bOp = MoveTemp(const_cast<FURL&>(b).Op);
    const_cast<FURL&>(a).Op = TArray<FString>();
    const_cast<FURL&>(b).Op = TArray<FString>();

    // Determine equality at this point.
    const bool result = a == b;

    // Restore their `Op` values.
    const_cast<FURL&>(a).Op = MoveTemp(aOp);
    const_cast<FURL&>(b).Op = MoveTemp(bOp);

    // Return the saved result.
    return result;
}

#undef LOCTEXT_NAMESPACE
