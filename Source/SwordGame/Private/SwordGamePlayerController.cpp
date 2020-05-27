// Fill out your copyright notice in the Description page of Project Settings.


#include "SwordGamePlayerController.h"

#include "Kismet/KismetSystemLibrary.h"

const FString ASwordGamePlayerController::GetNetworkURL(UObject* WorldContextObject)
{
	if (WorldContextObject)
	{
		if (UWorld* World = WorldContextObject->GetWorld())
		{
			return World->GetAddressURL();
		}
	}

	return FString();
}

void ASwordGamePlayerController::AcknowledgePossession(APawn* P)
{
	Super::AcknowledgePossession(P);

}

