// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityActor.h"

#include "SwordGameEditorEditorUtilityActor.generated.h"

/**
 *
 */
UCLASS()
class SWORDGAMEEDITOR_API ASwordGameEditorEditorUtilityActor : public AEditorUtilityActor
{
    GENERATED_BODY()

public:

    ASwordGameEditorEditorUtilityActor(const FObjectInitializer& objectInitializer);

private:

    /**
     * @brief Wrapper function for `Run()` that's callable from the details view.
     */
    UFUNCTION(CallInEditor, Category = "SwordGameEditor", meta = (DisplayName = "Run"))
    FORCEINLINE void RunCallInEditor()
    {
        Run();
    }
};
