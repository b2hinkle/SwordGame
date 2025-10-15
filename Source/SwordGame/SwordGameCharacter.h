// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "SwordGameCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;

UCLASS(config=Game)
class ASwordGameCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

public:
	ASwordGameCharacter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere, Category = Charge)
	bool bChargeMode;


protected:


	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

public:

	UFUNCTION(BlueprintCallable)
	void ServerRestartServer();

	UFUNCTION(BlueprintCallable)
	void ServerChangeServerMap(const FString& MapName);

	FORCEINLINE USpringArmComponent& GetCameraBoom() const
	{
		check(CameraBoom);
		return *CameraBoom;
	}

	FORCEINLINE UCameraComponent& GetFollowCamera() const
	{
		check(FollowCamera);
		return *FollowCamera;
	}

public:

	// UObject overrides.
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	// UObject overrides.

	// AActor overrides.
	virtual void BeginPlay() override;
	virtual void Tick(float deltaSeconds) override;
	virtual void PreReplication(IRepChangedPropertyTracker& changedPropertyTracker) override;
	// AActor overrides.

	// APawn overrides.
	virtual FRotator GetBaseAimRotation() const override;
	virtual FRotator GetViewRotation() const override;
	virtual void OnRep_Controller() override;
	// APawn overrides.

	FORCEINLINE uint16 GetRemoteViewYaw() const { return RemoteViewYaw16; }
	void SetRemoteViewYaw(float newValue);

protected:

	/**
	 * @brief This means to update any data or anything that depends on whether we are locally controlled or not.
	 */
	void UpdateLocalControllerDependentState();

private:

	/**
	 * @brief The yaw version of `APawn::RemoteViewPitch16`.
	 */
	UPROPERTY(Replicated)
	uint16 RemoteViewYaw16 = 0u;
};
