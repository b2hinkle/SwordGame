// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "GenericPlatform/ICursor.h"
#include "Framework/Application/IInputProcessor.h"

class FSlateApplication;
struct FKeyEvent;

DECLARE_MULTICAST_DELEGATE(FPlayerMenuInputPressDelegate)

extern FPlayerMenuInputPressDelegate SGOnPlayerMenuInputPress;

/** Input processor specifically for player menu to get around input mode UI.*/
class FPlayerMenuInputProcessor : public IInputProcessor
{
public:
	FPlayerMenuInputProcessor() = default;
	virtual ~FPlayerMenuInputProcessor() = default;

	//~ IInputProcess overrides
	bool HandleKeyDownEvent( FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent ) override;

	void Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor) override {}

	const TCHAR* GetDebugName() const override { return TEXT("PlayerMenuInputProcessor"); }
};
