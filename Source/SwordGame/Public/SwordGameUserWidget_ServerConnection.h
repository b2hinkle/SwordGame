// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SwordGameServerConnectionWidgetInterface.h"

#include "SwordGameUserWidget_ServerConnection.generated.h"

class UTextBlock;

/**
 *
 */
UCLASS()
class SWORDGAME_API USwordGameUserWidget_ServerConnection : public UUserWidget,
    public ISwordGameServerConnectionWidgetInterface
{
    GENERATED_BODY()

public:

    // ISwordGameServerConnectionWidgetInterface overrides.
    virtual void SetStatus(FText&& newValue) override;
    virtual void RetriggerStatusAttention(const float displayTimeSeconds) override;
    // ISwordGameServerConnectionWidgetInterface overrides.

protected:

    // UUserWidget overrides.
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
    virtual void NativeTick(const FGeometry& myGeometry, float deltaTime) override;
    // UUserWidget overrides.

private:

    void OnStatusAttentionTimeExpired(const float displayTimeSeconds);

private:

    FTimerHandle StatusAttentionTimerHandle;

    FTimerHandle StatusFadeTimerHandle;

    UPROPERTY(meta=(BindWidget))
    TObjectPtr<UTextBlock> StatusTextBlock = nullptr;

    UPROPERTY(EditDefaultsOnly)
    float StatusAttentionExpireFadeRate = 3.f;

    bool bIsStatusAttentionTimeExpired = true;
};
