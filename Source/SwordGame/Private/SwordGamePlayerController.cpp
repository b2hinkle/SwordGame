// Fill out your copyright notice in the Description page of Project Settings.

#include "SwordGamePlayerController.h"

FString ASwordGamePlayerController::GetNetworkURL(const UObject* worldContextObject)
{
    if (!worldContextObject)
    {
        return FString();
    }

    const UWorld* world = worldContextObject->GetWorld();
    if (!world)
    {
        return FString();
    }

    return world->GetAddressURL();
}

void ASwordGamePlayerController::RestartWorld() const
{
    HandleRestartWorld();
}

void ASwordGamePlayerController::TravelToWorld(const FString& mapName) const
{
    HandleTravelToWorld(mapName);
}

void ASwordGamePlayerController::ForceReconnectAllPlayers() const
{
    HandleForceReconnectAllPlayers();
}
