// Fill out your copyright notice in the Description page of Project Settings.

#include "SwordGameBlueprintFunctionLibrary.h"

#include "PlayerMenuInputProcessor.h"

void USwordGameBlueprintFunctionLibrary::ExecuteOnPlayerMenuInputPress(const UObject* ctxObj, FPlayerMenuInputPressDynamicDelegate del)
{
    SGOnPlayerMenuInputPress.AddWeakLambda(ctxObj, [del = MoveTemp(del)]()
        {
            del.ExecuteIfBound();
        });
}

void USwordGameBlueprintFunctionLibrary::OnPlayerMenuInputPress_RemoveByCtxObj(const UObject* ctxObj)
{
    SGOnPlayerMenuInputPress.RemoveAll(ctxObj);
}
