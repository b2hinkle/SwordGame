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
    void HandleTravelToWorld(const FName& mapName) const;

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

    UFUNCTION(BlueprintImplementableEvent)
    void HandleDisableInvisibilityForTheInnocent() const;

public:

    UFUNCTION(BlueprintPure, meta = (WorldContext = worldContextObject))
    static FString GetNetworkURL(const UObject* worldContextObject);

    static TArray<FString> GetAllAutoCompleteMapNames();

public:

    UFUNCTION(Exec)
    void NinjaJoin(const FName& address) const;

    UFUNCTION(Exec)
    void NinjaRestart() const;

    UFUNCTION(Exec)
    void NinjaTravel(const FName& mapName) const;

    UFUNCTION(Exec)
    void NinjaForceReconnectAllPlayers() const;

    UFUNCTION(Exec)
    void NinjaToggleModTools() const;

    UFUNCTION(Exec)
    void NinjaEnableFriendlyFireForRandomPlayer() const;

    /**
     * @note Requires mod tools enabled.
     * @see `NinjaToggleModTools()`.
     */
    UFUNCTION(Exec)
    void NinjaBallz(const int howMany) const;

    /**
     * @note Requires mod tools enabled.
     * @see `NinjaToggleModTools()`.
     */
    UFUNCTION(Exec)
    void NinjaClearObjects() const;

    /**
     * @note Requires mod tools enabled.
     * @see `NinjaToggleModTools()`.
     */
    UFUNCTION(Exec)
    void NinjaLaunchSelf(const float force) const;

    /**
     * @todo Change this to a setter rather than a toggle.
     */
    UFUNCTION(Exec)
    void NinjaToggleSwitchViewTargetOnFriendlyFire() const;

    UFUNCTION(Exec)
    void NinjaMrIncredi1() const;

    UFUNCTION(Exec)
    void NinjaMrIncredi2() const;

    UFUNCTION(Exec)
    void NinjaMrIncredi3() const;

    /**
     * @note Requires mod tools enabled.
     * @see `NinjaToggleModTools()`.
     */
    UFUNCTION(Exec)
    void NinjaSetPawnRelative3DScale(const float valueX, const float valueY, const float valueZ) const;

    UFUNCTION(Exec)
    void NinjaGiveChargeAbility() const;

    UFUNCTION(Exec)
    void NinjaGiveMineAbility() const;

    UFUNCTION(Exec)
    void NinjaGiveTeleportAbility() const;

    UFUNCTION(Exec)
    void NinjaDisableInvisibilityForTheInnocent() const;
};
