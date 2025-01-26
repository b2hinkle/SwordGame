// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "SwordGamePlayerController.generated.h"

/**
 *
 */
UCLASS()
class SWORDGAME_API ASwordGamePlayerController : public APlayerController
{
    GENERATED_BODY()

protected:

    UFUNCTION(BlueprintImplementableEvent)
    void HandleRestartWorld() const;

    UFUNCTION(BlueprintImplementableEvent)
    void HandleTravelToWorld(const FString& mapName) const;

    UFUNCTION(BlueprintImplementableEvent)
    void HandleForceReconnectAllPlayers() const;

    UFUNCTION(BlueprintImplementableEvent)
    void HandleToggleModTools() const;

    UFUNCTION(BlueprintImplementableEvent)
    void HandleEnableFriendlyFireForRandomPlayer() const;

    UFUNCTION(BlueprintImplementableEvent)
    void HandleBallz(const int howMany) const;

    UFUNCTION(BlueprintImplementableEvent)
    void HandleClearObjects() const;

    UFUNCTION(BlueprintImplementableEvent)
    void HandleLaunchSelf(const float force) const;

    UFUNCTION(BlueprintImplementableEvent)
    void HandleToggleSwitchViewTargetOnFriendlyFire() const;

    UFUNCTION(BlueprintImplementableEvent)
    void HandleMrIncredi1() const;

    UFUNCTION(BlueprintImplementableEvent)
    void HandleMrIncredi2() const;

    UFUNCTION(BlueprintImplementableEvent)
    void HandleMrIncredi3() const;

    UFUNCTION(BlueprintImplementableEvent)
    void HandleSetPawnRelative3DScale(const float valueX, const float valueY, const float valueZ) const;

    UFUNCTION(BlueprintImplementableEvent)
    void HandleGiveChargeAbility() const;

    UFUNCTION(BlueprintImplementableEvent)
    void HandleGiveMineAbility() const;

    UFUNCTION(BlueprintImplementableEvent)
    void HandleGiveTeleportAbility() const;

public:

    UFUNCTION(BlueprintPure, meta = (WorldContext = worldContextObject))
    static FString GetNetworkURL(const UObject* worldContextObject);

    static TArray<FString> GetAllAutoCompleteMapNames();

public:

    UFUNCTION(Exec)
    void RestartWorld() const;

    UFUNCTION(Exec)
    void TravelToWorld(const FString& mapName) const;

    UFUNCTION(Exec)
    void ForceReconnectAllPlayers() const;

    UFUNCTION(Exec)
    void ToggleModTools() const;

    UFUNCTION(Exec)
    void EnableFriendlyFireForRandomPlayer() const;

    UFUNCTION(Exec)
    void Ballz(const int howMany) const;

    UFUNCTION(Exec)
    void ClearObjects() const;

    UFUNCTION(Exec)
    void LaunchSelf(const float force) const;

    /**
     * @todo Change this to a setter rather than a toggle.
     */
    UFUNCTION(Exec)
    void ToggleSwitchViewTargetOnFriendlyFire() const;

    UFUNCTION(Exec)
    void MrIncredi1() const;

    UFUNCTION(Exec)
    void MrIncredi2() const;

    UFUNCTION(Exec)
    void MrIncredi3() const;

    UFUNCTION(Exec)
    void SetPawnRelative3DScale(const float valueX, const float valueY, const float valueZ) const;

    UFUNCTION(Exec)
    void GiveChargeAbility() const;

    UFUNCTION(Exec)
    void GiveMineAbility() const;

    UFUNCTION(Exec)
    void GiveTeleportAbility() const;
};
