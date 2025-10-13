// Fill out your copyright notice in the Description page of Project Settings.

#include "SwordGameServerConnectionWidgetInterface.h"

#include "Blueprint/UserWidget.h"

UUserWidget& ISwordGameServerConnectionWidgetInterface::GetUserWidget()
{
    UObject* object = _getUObject();
    check(object);
    UUserWidget* userWidget = Cast<UUserWidget>(object);
    check(userWidget);
    return *userWidget;
}
