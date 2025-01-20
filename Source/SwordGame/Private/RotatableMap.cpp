// Fill out your copyright notice in the Description page of Project Settings.


#include "RotatableMap.h"

#include "Kismet/KismetMathLibrary.h"

#include "Kismet/KismetSystemLibrary.h"


// Sets default values
ARotatableMap::ARotatableMap()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetReplicates(true);
	AngleAxis = 0;

}

// Called when the game starts or when spawned
void ARotatableMap::BeginPlay()
{
	Super::BeginPlay();
	
	
}

// Called every frame
void ARotatableMap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bShouldRotate)
	{
		if (rotAxis == rotAxisX)
		{
			//if (UKismetMathLibrary::Abs(rotationProgress) < UKismetMathLibrary::Abs(FQuat(1, 0, 0, 0).X)/*UKismetMathLibrary::Abs(degsToStopRotation)*/)
			//{
				Rotate(AxisOfRotation, Speed, DeltaTime);
			//}
			//else
			//{
			//	//SetActorRotation(GoalRotation);
			//	ResetForNextRotation();
			//}
		}
		if (rotAxis == rotAxisY)
		{
			//if (UKismetMathLibrary::Abs(rotationProgress) < UKismetMathLibrary::Abs(degsToStopRotation))
			//{
				Rotate(AxisOfRotation, Speed, DeltaTime);
			//}
			//else
			//{
			//	ResetForNextRotation();
			//}
		}
		if (rotAxis == rotAxisZ)
		{
			//if (UKismetMathLibrary::Abs(rotationProgress) < UKismetMathLibrary::Abs(degsToStopRotation))
			//{
				Rotate(AxisOfRotation, Speed, DeltaTime);
			//}
			//else
			//{
			//	ResetForNextRotation();
			//}
		}
	}
	

}

void ARotatableMap::Rotate(FVector AxisOR, float speed, float deltaTime)
{
	// declare arbitrary vector point in the world to circle around
	FVector NewLocation = Centroid;



	// angle increases
	AngleAxis = AngleAxis + (speed * deltaTime);
	//rotationProgress = rotationProgress + (speed * deltaTime);
	rotationProgress = rotationProgress + 0.001f;

	// prevent number from growind indefinitely
	if (AngleAxis > 360.0f)
	{
		AngleAxis = 0;
		//AngleAxis = AngleAxis - 360.f;
	}

	FVector RotateValue = Radius.RotateAngleAxis(AngleAxis, AxisOR);

	NewLocation.X += RotateValue.X;
	NewLocation.Y += RotateValue.Y;
	NewLocation.Z += RotateValue.Z;

	SetActorLocation(NewLocation);
	lookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Centroid);
	if (rotAxis == rotAxisX)
	{
		AddActorWorldRotation(FRotator(0, 0, -(speed * deltaTime)).Quaternion());
	}
	if (rotAxis == rotAxisY)
	{
		AddActorWorldRotation(FRotator(-(speed * deltaTime), 0, 0).Quaternion());
	}
	if (rotAxis == rotAxisZ)
	{
		AddActorWorldRotation(FRotator(0, (speed * deltaTime), 0).Quaternion());
	}
}

void ARotatableMap::PerformRotationByAmount(FVector AxisOR, float speed, float degs)
{
	initialPos = GetActorLocation();
	this->Speed = speed;
	this->AxisOfRotation = AxisOR;
	if (AxisOR == FVector(1.f, 0.f, 0.f))
	{
		//GoalRotation = GetActorRotation() + FRotator(0.f, 0.f, degs);
		degsToStopRotation = degs + GetActorRotation().Vector().X;
		rotAxis = rotAxisX;
	}
	if (AxisOR == FVector(0.f, 1.f, 0.f))
	{
		//GoalRotation = GetActorRotation() + FRotator(degs, 0.f, 0.f);
		degsToStopRotation = degs + GetActorRotation().Vector().Y;
		rotAxis = rotAxisY;
	}
	if (AxisOR == FVector(0.f, 0.f, 1.f))
	{
		//GoalRotation = GetActorRotation() + FRotator(0.f, degs, 0.f);
		degsToStopRotation = degs + GetActorRotation().Vector().Z;
		rotAxis = rotAxisZ;
	}
	bShouldRotate = true;
}

void ARotatableMap::OnLevelBlueprintInjectCentroid(FVector centroid, FVector axisOfRotation)
{
	this->Centroid = centroid;
	this->AxisOfRotation = axisOfRotation;
	this->AxisOfRotation.Normalize();
	distanceFromCentroid = (GetActorLocation() - centroid).Size();

	lookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), centroid);
	Radius = (distanceFromCentroid * lookAtRotation.Vector()) * FVector(-1.f, -1.f, -1.f);
}

void ARotatableMap::ResetForNextRotation()
{
	bShouldRotate = false;
	rotationProgress = 0.f;
	degsToStopRotation = 0.f;
	initialPos = FVector(0.f, 0.f, 0.f);
}

