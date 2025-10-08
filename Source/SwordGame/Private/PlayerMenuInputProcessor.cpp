// Copyright Epic Games, Inc. All Rights Reserved.

#include "PlayerMenuInputProcessor.h"

FPlayerMenuInputPressDelegate SGOnPlayerMenuInputPress;

bool IsPlayerMenuKey(const FKey& key)
{
	return key == EKeys::Escape;
}

bool FPlayerMenuInputProcessor::HandleKeyDownEvent( FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent )
{
	if (IsPlayerMenuKey(InKeyEvent.GetKey()))
	{
		SGOnPlayerMenuInputPress.Broadcast();
		return true; // Intercept the event
	}
	return false;
}

