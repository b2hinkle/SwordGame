// Fill out your copyright notice in the Description page of Project Settings.

#include "SwordGameEditorBlueprintFunctionLibrary.h"

UWorld* USwordGameEditorBlueprintFunctionLibrary::GetWorldFromActor(const AActor* inActor)
{
    if (!inActor)
    {
        return nullptr;
    }

    return inActor->GetWorld();
}

UWorld* USwordGameEditorBlueprintFunctionLibrary::ResolveWorld(
    const FName& inPackageName,
    const FName& inAssetName)
{
    UObject* resolvedObject =
        ResolveObject(inPackageName, inAssetName);
    if (!resolvedObject)
    {
        return nullptr;
    }

    UWorld* resolvedObjectCasted = Cast<UWorld>(resolvedObject);
    ensure(resolvedObjectCasted);

    return resolvedObjectCasted;
}

UObject* USwordGameEditorBlueprintFunctionLibrary::ResolveObject(
    const FName& inPackageName,
    const FName& inAssetName,
    FString inSubPathString)
{
    UObject* resolvedObject =
        FSoftObjectPath(inPackageName, inAssetName, MoveTemp(inSubPathString)).ResolveObject();

    return resolvedObject;
}
