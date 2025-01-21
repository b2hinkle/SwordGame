// Fill out your copyright notice in the Description page of Project Settings.

#include "SwordGameEditorEditorUtilityActor.h"

ASwordGameEditorEditorUtilityActor::ASwordGameEditorEditorUtilityActor(const FObjectInitializer& objectInitializer)
    : Super(objectInitializer)
{
    bIsEditorOnlyActor = true;
    bIsSpatiallyLoaded = false;
}
