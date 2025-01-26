// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "SwordGameEditorBlueprintFunctionLibrary.generated.h"

class ULightComponentBase;

/**
 *
 */
UCLASS()
class SWORDGAMEEDITOR_API USwordGameEditorBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:

    UFUNCTION(BlueprintPure, Category = "SwordGameEditor")
    static UWorld* GetWorld(
        const UObject* inWorldContextObject);

    UFUNCTION(BlueprintPure, Category = "SwordGameEditor")
    static UWorld* ResolveWorld(
        const FName& inPackageName,
        const FName& inAssetName);

    UFUNCTION(BlueprintPure, Category = "SwordGameEditor")
    static UObject* ResolveObject(
        const FName& inPackageName,
        const FName& inAssetName,
        FString inSubPathString = TEXT(""));

    UFUNCTION(BlueprintCallable, Category = "SwordGameEditor")
    static void SetLightComponentIntensityDirect(
        ULightComponentBase* inLightComponent,
        const float inValue);
};
