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
	
public:
	UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject"))
		static const FString GetNetworkURL(UObject* WorldContextObject);

		virtual void AcknowledgePossession(class APawn* P);
};
