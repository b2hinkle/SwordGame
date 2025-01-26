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

public:

    UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject"))
    static FString GetNetworkURL(const UObject* worldContextObject);

public:

    UFUNCTION(Exec)
    void RestartWorld() const;

    UFUNCTION(Exec)
    void TravelToWorld(const FString& mapName) const;

    UFUNCTION(Exec)
    void ForceReconnectAllPlayers() const;
};
