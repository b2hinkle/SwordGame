// Fill out your copyright notice in the Description page of Project Settings.

#include "SwordGameServerConnectionSubsystem.h"

#include "Engine/World.h"
#include "SwordGameGameUserSettings.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Blueprint/UserWidget.h"
#include "SwordGameServerConnectionWidgetInterface.h"
#include "Engine/NetworkDelegates.h"

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

    bool SwordGame_ServerConnection_ShouldAttemptAutoConnectOnNetworkFailure = false;

    FAutoConsoleVariableRef CVar_SwordGame_ServerConnection_ShouldAttemptAutoConnectOnNetworkFailure(
        TEXT("SwordGame.ServerConnection.ShouldAttemptAutoConnectOnNetworkFailure"),
        SwordGame_ServerConnection_ShouldAttemptAutoConnectOnNetworkFailure,
        TEXT("If true, attempt to re-auto-connect on network failure.")
    );
}

#define LOCTEXT_NAMESPACE "SwordGameServerConnectionSubsystem"

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
}

void USwordGameServerConnectionSubsystem::Deinitialize()
{
    GetGameInstanceChecked().GetTimerManager().ClearTimer(ConnectionAttemptTimerHandle);

    FCoreUObjectDelegates::PreLoadMapWithContext.Remove(PreLoadMapWithContextDelegateHandle);

    check(GEngine);
    GEngine->OnTravelFailure().Remove(OnTravelFailureDelegateHandle);

    FWorldDelegates::OnStartGameInstance.Remove(OnStartGameInstanceDelegateHandle);

    Super::Deinitialize();
}

void USwordGameServerConnectionSubsystem::KickOffAutoConnectSequence()
{
    USwordGameGameUserSettings& myGameUserSettings = USwordGameGameUserSettings::GetChecked();
    ensureMsgf(myGameUserSettings.GetIsAutoConnectEnabled(), TEXT("This should not have been called."));

    constexpr ETravelType travelType = ETravelType::TRAVEL_Absolute;

    KickOffConnectionSequence(
        myGameUserSettings.GetAutoConnectURL(),
        travelType,
        myGameUserSettings.GetAutoConnectNumTries());
}

void USwordGameServerConnectionSubsystem::KickOffConnectionSequence(FString url, const ETravelType travelType, const uint8 numTimesToTry)
{
    const UWorld* world = GetWorld();
    checkf(world, TEXT("This should not have been called at this state. It's expected for the world to be valid."));

    ensureMsgf(!IsCurrentlyInConnectionSequence(), TEXT("This should be the first connection try in the sequence."));
    ensureMsgf(GetCurrentConnectionSequenceTry() == 0u, TEXT("This should be the first connection try in the sequence."));

    CurrentConnectionSequenceURL = MoveTemp(url);
    CurrentConnectionSequenceTravelType = travelType;
    CurrentConnectionSequenceNumTimesToTry = numTimesToTry;
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
    ResetConnectionSequenceData();
}

void USwordGameServerConnectionSubsystem::CancelCurrentConnectionSequence()
{
    check(GEngine);
    GEngine->CancelAllPending();

    GetGameInstanceChecked().GetTimerManager().ClearTimer(ConnectionAttemptTimerHandle);
    ResetConnectionSequenceData();
}

uint8 USwordGameServerConnectionSubsystem::GetDefaultConnectionSequenceNumTimesToTry()
{
    return USwordGameGameUserSettings::GetChecked().GetAutoConnectNumTries();
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
        FText connectingToServerText = LOCTEXT("ServerConnectionWidget_Status_KickOffConnectionSequence_FirstTry", "Connecting to server...");

        if (CurrentConnectionSequenceTry <= 1u)
        {
            serverConnectionWidget->SetStatus(MoveTemp(connectingToServerText));
        }
        else
        {
            serverConnectionWidget->SetStatus(
                FText::Format(
                    LOCTEXT("ServerConnectionWidget_Status_KickOffConnectionSequence_NextTries", "{0}\n(attempt: {1}/{2})"),
                    MoveTemp(connectingToServerText),
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
                    LOCTEXT("ServerConnectionWidget_Status_TryRetryConnect_Failed", "Failed to connect after {0} attempts"),
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
        return;
    }

    if (SwordGame_ServerConnection_ShouldAttemptAutoConnectOnNetworkFailure)
    {
        UWorld* thisWorld = GetWorld();
        if (ensureMsgf(thisWorld, TEXT("I think this should always be valid, but not fully confident.")))
        {
            // TODO: @Christian: [todo] Implement auto-connect on network failure. First we want to make sure we came from the same host as the auto-connection host.
        }
    }
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
    if (worldContext.LastURL != CurrentConnectionSequenceLastURL)
    {
        UE_LOG(LogSwordGameServerConnectionSubsystem, Warning, TEXT("%s"),
            TStringBuilder<512>(EInPlace::InPlace,
                TEXT("The last URL has somehow changed during our connection sequence (maybe caused by an interfering world travel?). Canceling the current connection sequence.")
            ).ToString()
        );

        CancelCurrentConnectionSequence();
        return;
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
        serverConnectionWidget->SetStatus(LOCTEXT("ServerConnectionWidget_Status_Success", "Connected"));
        serverConnectionWidget->RetriggerStatusAttention(1.f);
    }

    EndCurrentConnectionSequence();
}

void USwordGameServerConnectionSubsystem::OnConnectionAttemptTimeout()
{
    OnConnectionFailureDuringConnectionSequence();
}

void USwordGameServerConnectionSubsystem::ResetConnectionSequenceData()
{
    CurrentConnectionSequenceTry = 0u;
    CurrentConnectionSequenceURL.Empty();
    CurrentConnectionSequenceNumTimesToTry = 0u;
}

#undef LOCTEXT_NAMESPACE
