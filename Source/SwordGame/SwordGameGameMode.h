// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SwordGameGameMode.generated.h"

UCLASS(minimalapi)
class ASwordGameGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ASwordGameGameMode();

protected:
	virtual void BeginPlay() override;
};



