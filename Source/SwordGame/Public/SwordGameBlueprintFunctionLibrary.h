// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "SwordGameBlueprintFunctionLibrary.generated.h"

DECLARE_DYNAMIC_DELEGATE(FPlayerMenuInputPressDynamicDelegate);

/**
 *
 */
UCLASS()
class SWORDGAME_API USwordGameBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:

    UFUNCTION(BlueprintCallable, Category = "SwordGame")
    static void ExecuteOnPlayerMenuInputPress(const UObject* ctxObj, FPlayerMenuInputPressDynamicDelegate del);

    UFUNCTION(BlueprintCallable, Category = "SwordGame")
    static void OnPlayerMenuInputPress_RemoveByCtxObj(const UObject* ctxObj);
};
