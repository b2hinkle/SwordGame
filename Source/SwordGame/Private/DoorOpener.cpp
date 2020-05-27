// Fill out your copyright notice in the Description page of Project Settings.


#include "DoorOpener.h"

// Sets default values
ADoorOpener::ADoorOpener()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CylinderThing = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CylanderThing"));
}

// Called when the game starts or when spawned
void ADoorOpener::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADoorOpener::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

