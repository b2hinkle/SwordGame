// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/GameInstance.h"

#include "SwordGameServerConnectionSubsystem.generated.h"

class UUserWidget;
class ISwordGameServerConnectionWidgetInterface;
class UPendingNetGame;
class UNetDriver;

/**
 * @brief Provides functionality for managing connecting to a server with retries, including an auto-connnect feature on startup (using `USwordGameUserSettings`).
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

    UFUNCTION(BlueprintCallable)
    void KickOffConnectionSequence(FString url, const ETravelType travelType, const uint8 numTimesToTry);

    UFUNCTION(BlueprintCallable)
    FORCEINLINE void KickOffConnectionSequenceWithDefaultNumTimesToTry(FString url, const ETravelType travelType)
    {
        KickOffConnectionSequence(MoveTemp(url), travelType, GetDefaultConnectionSequenceNumTimesToTry());
    }

    UFUNCTION(BlueprintCallable)
    void EndCurrentConnectionSequence();

    UFUNCTION(BlueprintCallable)
    void CancelCurrentConnectionSequence();

    UFUNCTION(BlueprintPure)
    static uint8 GetDefaultConnectionSequenceNumTimesToTry();

private:

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

    void OnPendingNetGameConnectionCreated(UPendingNetGame& pendingNetGame);

    void OnPreLoadMap(const FWorldContext& worldContext, const FStringView mapName);

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

    void ResetConnectionSequenceData();

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

private:

    FURL CurrentConnectionSequenceLastURL;

    UPROPERTY(Config)
    TSoftObjectPtr<UUserWidget> ServerConnectionUserWidgetClassSoft;

    FString CurrentConnectionSequenceURL;

    FTimerHandle ConnectionAttemptTimerHandle;

    FDelegateHandle OnStartGameInstanceDelegateHandle;

    FDelegateHandle OnTravelFailureDelegateHandle;

    FDelegateHandle OnNetworkFailureDelegateHandle;

    FDelegateHandle OnPendingNetGameConnectionCreatedDelegateHandle;

    FDelegateHandle PreLoadMapWithContextDelegateHandle;

    UPROPERTY(Transient)
    TScriptInterface<ISwordGameServerConnectionWidgetInterface> ServerConnectionUserWidgetInstance;

    ETravelType CurrentConnectionSequenceTravelType = static_cast<ETravelType>(0u);

    uint8 CurrentConnectionSequenceNumTimesToTry;

    uint8 CurrentConnectionSequenceTry = 0u;
};
