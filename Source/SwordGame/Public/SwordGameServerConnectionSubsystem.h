// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/GameInstance.h"
#include "SwordGameGameUserSettings.h"

#include "SwordGameServerConnectionSubsystem.generated.h"

class UUserWidget;
class ISwordGameServerConnectionWidgetInterface;
class UPendingNetGame;
class UNetDriver;

USTRUCT(BlueprintType)
struct FConnectionSequenceArgs
{
    GENERATED_BODY();

public:

    FConnectionSequenceArgs() = default;
    FConnectionSequenceArgs(FString&& url, const ETravelType travelType)
        : URL(MoveTemp(url))
        , TravelType(travelType)
    {
    }
    FConnectionSequenceArgs(const FURL& url)
        // Actual URLs are not relative to anything, so travel to it absolutely.
        : FConnectionSequenceArgs(url.ToString(), ETravelType::TRAVEL_Absolute)
    {
    }

    FORCEINLINE FConnectionSequenceArgs&& SetNumTimesToTry(const uint8 newValue) &&
    {
        NumTimesToTry = newValue;
        return MoveTemp(*this);
    }

    FORCEINLINE FConnectionSequenceArgs&& SetConnectingToServerStatusText(FText&& newValue) &&
    {
        ConnectingToServerStatusText = MoveTemp(newValue);
        return MoveTemp(*this);
    }

    FORCEINLINE FConnectionSequenceArgs&& SetFailedToConnectFormattedStatusText(FText&& newValue) &&
    {
        FailedToConnectFormattedStatusText = MoveTemp(newValue);
        return MoveTemp(*this);
    }

    static FText GetDefaultConnectingToServerStatusText();

    static FText GetDefaultFailedToConnectFormattedStatusText();

public:

    FText ConnectingToServerStatusText = GetDefaultConnectingToServerStatusText();

    FText FailedToConnectFormattedStatusText = GetDefaultFailedToConnectFormattedStatusText();

    FString URL;

    ETravelType TravelType = ETravelType::TRAVEL_Absolute;

    uint8 NumTimesToTry = USwordGameGameUserSettings::GetChecked().GetAutoConnectNumTries();
};

/**
 * @brief Provides functionality for managing connecting to a server with retries, including an auto-connnect feature on startup (using `USwordGameUserSettings`).
 *
 * Auto-connect is the attempt to connect to a specified server on game startup.
 *
 * Auto-reconnect is the attempt to reconnect to the most recently connected to server whenever the client gets disconnected.
 */
UCLASS(Config="SwordGameServerConnectionSubsystem")
class SWORDGAME_API USwordGameServerConnectionSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:

    // USubsystem overrides.
    virtual void Initialize(FSubsystemCollectionBase& collection) override;
    virtual void Deinitialize() override;
    // USubsystem overrides.

    FORCEINLINE UGameInstance& GetGameInstanceChecked() const
    {
        UGameInstance* gameInstance = GetGameInstance();
        check(gameInstance);
        return *gameInstance;
    }

    UFUNCTION(BlueprintPure)
    FORCEINLINE uint8 GetCurrentConnectionSequenceTry() const
    {
        return CurrentConnectionSequenceTry;
    }

    UFUNCTION(BlueprintPure)
    FORCEINLINE bool IsCurrentlyInConnectionSequence() const
    {
        if (GetCurrentConnectionSequenceTry() <= 0u)
        {
            // The number indicates we haven't started any auto-connect tries.
            return false;
        }

        // If the value is positive, there must be a current auto-connect in progress.
        return true;
    }

    UFUNCTION(BlueprintCallable)
    void KickOffAutoConnectSequence();

    void TryKickOffAutoReconnectSequence(const FURL& lastRemoteURL);

    void KickOffAutoReconnectSequence(const FURL& lastRemoteURL);

    void KickOffConnectionSequence(FConnectionSequenceArgs&& args);

    UFUNCTION(BlueprintCallable)
    void EndCurrentConnectionSequence();

    UFUNCTION(BlueprintPure)
    static uint8 GetDefaultConnectionSequenceNumTimesToTry();

    static FText GetConnectedConnectionStatusText();

    static FText GetDisconnectedConnectionStatusText();

private:

    UFUNCTION(BlueprintCallable)
    void BPKickOffConnectionSequenceUsingStringURL(FConnectionSequenceArgs args)
    {
        KickOffConnectionSequence(MoveTemp(args));
    }

    void OnStartGameInstance(UGameInstance& gameInstance);

    /**
     * @brief Loads, creates, and adds the server connection user widget.
     */
    void LoadCreateAddServerConnectionUserWidget(UGameInstance& gameInstance);

    void TryConnect();

    void TryRetryConnect();

    void OnConnectionFailureDuringConnectionSequence();

    void OnTravelFailure(UWorld* world, const ETravelFailure::Type failureType, const FStringView errorString);

    void OnNetworkFailure(UWorld* world, UNetDriver* netDriver, const ENetworkFailure::Type failureType, const FStringView errorString);

    void OnHandleDisconnect(UWorld* inWorld, UNetDriver* netDriver);

    void OnPendingNetGameConnectionCreated(UPendingNetGame& pendingNetGame);

    void OnPreLoadMap(const FWorldContext& worldContext, const FStringView mapName);

    void OnPostLoadMap(UWorld* loadedWorld);

    void OnPostLoadMapAfterDisconnect(UWorld* loadedWorld);

    void OnTravelFailureDuringOurConnectionSequence(
        UWorld* world,
        const ETravelFailure::Type failureType,
        const FStringView& errorString);

    void OnNetworkFailureDuringOurConnectionSequence(
        UWorld* world,
        UNetDriver* netDriver,
        const ENetworkFailure::Type failureType,
        const FStringView& errorString);

    void OnPendingNetGameConnectionCreatedDuringOurConnectionSequence(UPendingNetGame& pendingNetGame);

    void OnPreLoadMapDuringOurConnectionSequence(const FWorldContext& worldContext, const FStringView& mapName);

    void OnConnectionSequenceSuccess();

    void OnConnectionAttemptTimeout();

    void ResetCurrentConnectionSequenceData();

    FORCEINLINE ISwordGameServerConnectionWidgetInterface* GetServerConnectionUserWidgetInstance() const
    {
        // Get by object pointer to avoid any kind of bug with the engine where it doesn't set the interface pointer when
        // set from blueprint script.
        UObject* theObject = ServerConnectionUserWidgetInstance.GetObject();
        ISwordGameServerConnectionWidgetInterface* theInterface = Cast<ISwordGameServerConnectionWidgetInterface>(theObject);
        return theInterface;
    }

    FORCEINLINE FURL GetCurrentConnectionSequenceURL() const
    {
        return FURL(
            &const_cast<FURL&>(CurrentConnectionSequenceLastURL),
            CurrentConnectionSequenceURL.GetCharArray().GetData(),
            CurrentConnectionSequenceTravelType
        );
    }

    static bool AreURLsEqualDisregardingOps(const FURL& a, const FURL& b);

private:

    FURL CurrentConnectionSequenceLastURL;

    UPROPERTY(Config)
    TSoftObjectPtr<UUserWidget> ServerConnectionUserWidgetClassSoft;

    FText CurrentConnectionSequenceConnectingToServerStatusText;

    FText CurrentConnectionSequenceFailedToConnectFormattedStatusText;

    FString CurrentConnectionSequenceURL;

    FTimerHandle ConnectionAttemptTimerHandle;

    FDelegateHandle OnStartGameInstanceDelegateHandle;

    FDelegateHandle OnTravelFailureDelegateHandle;

    FDelegateHandle OnNetworkFailureDelegateHandle;

    FDelegateHandle HandleDisconnectDelegateHandle;

    FDelegateHandle OnPendingNetGameConnectionCreatedDelegateHandle;

    FDelegateHandle PreLoadMapWithContextDelegateHandle;

    FDelegateHandle PostLoadMapWithWorldDelegateHandle;

    UPROPERTY(Transient)
    TScriptInterface<ISwordGameServerConnectionWidgetInterface> ServerConnectionUserWidgetInstance;

    ETravelType CurrentConnectionSequenceTravelType = static_cast<ETravelType>(0u);

    uint8 CurrentConnectionSequenceNumTimesToTry = 0u;

    uint8 CurrentConnectionSequenceTry = 0u;

    uint8 NumTimesAutoReconnected = 0u;

    bool bIsCurrentlyDisconnectedAndScheduledToReturnToDefaultMap = false;
};
