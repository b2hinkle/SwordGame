// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "RotatableMap.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FinishedRotating);

UCLASS()
class SWORDGAME_API ARotatableMap : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARotatableMap();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Rotate(FVector AxisOR, float speed, float deltaTime);
	UFUNCTION(BlueprintCallable)
		void PerformRotationByAmount(FVector AxisOR, float speed, float degs);

	UFUNCTION(BlueprintCallable)
		void OnLevelBlueprintInjectCentroid(FVector Centroid, FVector AxisOfRotation);

	FRotator lookAtRotation;
	FVector AxisOfRotation;

	float distanceFromCentroid;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector Centroid;

	FVector Radius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bShouldRotate = false;
	float degsToStopRotation = 0.f;
	FVector initialPos = FVector(0.f, 0.f, 0.f);
	float rotationProgress;
	FRotator GoalRotation;
	enum RotAxis
	{
		rotAxisX,
		rotAxisY,
		rotAxisZ
	};
	RotAxis rotAxis = rotAxisX;
	float Speed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AngleAxis;

	void ResetForNextRotation();
};
