// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "SwordGameServerConnectionWidgetInterface.generated.h"

class UUserWidget;

UINTERFACE(MinimalAPI)
class USwordGameServerConnectionWidgetInterface : public UInterface
{
    GENERATED_BODY()
};

/**
 *
 */
class SWORDGAME_API ISwordGameServerConnectionWidgetInterface
{
    GENERATED_BODY()

public:

    UUserWidget& GetUserWidget();

    /**
     * @brief Set the current status text.
     */
    virtual void SetStatus(FText&& newValue) = 0;

    /**
     * @brief If the visual has faded away after some time, re-show it now.
     */
    virtual void RetriggerStatusAttention(const float displayTimeSeconds) = 0;
};
