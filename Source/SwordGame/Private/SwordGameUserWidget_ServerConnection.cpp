// Fill out your copyright notice in the Description page of Project Settings.

#include "SwordGameUserWidget_ServerConnection.h"

#include "Components/TextBlock.h"

void USwordGameUserWidget_ServerConnection::SetStatus(FText&& newValue)
{
    check(StatusTextBlock);
    StatusTextBlock->SetText(MoveTemp(newValue));
}

void USwordGameUserWidget_ServerConnection::RetriggerStatusAttention(const float displayTimeSeconds)
{
    UGameInstance* gameInstance = GetGameInstance();
    check(gameInstance);

    bIsStatusAttentionTimeExpired = false;

    check(StatusTextBlock);
    StatusTextBlock->SetRenderOpacity(1.f);

    constexpr bool shouldLoop = false;
    gameInstance->GetTimerManager().SetTimer(
        StatusAttentionTimerHandle,
        [this, displayTimeSeconds]()
        {
            OnStatusAttentionTimeExpired(displayTimeSeconds);
        },
        displayTimeSeconds,
        shouldLoop
    );
}

void USwordGameUserWidget_ServerConnection::NativeConstruct()
{
    Super::NativeConstruct();
}

void USwordGameUserWidget_ServerConnection::NativeDestruct()
{
    UGameInstance* gameInstance = GetGameInstance();
    check(gameInstance);

    gameInstance->GetTimerManager().ClearTimer(StatusFadeTimerHandle);
    gameInstance->GetTimerManager().ClearTimer(StatusAttentionTimerHandle);

    Super::NativeDestruct();
}

void USwordGameUserWidget_ServerConnection::NativeTick(const FGeometry& myGeometry, float deltaTime)
{
    if (bIsStatusAttentionTimeExpired)
    {
        if (StatusTextBlock->GetRenderOpacity() > 0.f)
        {
            check(StatusTextBlock);
            StatusTextBlock->SetRenderOpacity(StatusTextBlock->GetRenderOpacity() - (deltaTime / StatusAttentionExpireFadeRate));
        }
    }
}

void USwordGameUserWidget_ServerConnection::OnStatusAttentionTimeExpired(const float displayTimeSeconds)
{
    bIsStatusAttentionTimeExpired = true;
}
