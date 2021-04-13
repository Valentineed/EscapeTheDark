// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "UnrealProjectCharacter.generated.h"

class UPointLightComponent;
class UInputComponent;

UCLASS(config=Game)
class AUnrealProjectCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	class USkeletalMeshComponent* Mesh1P;

	/**/
	UPROPERTY(VisibleDefaultsOnly, Category = Light)
	class UPointLightComponent* Light = nullptr;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;

	/** Motion controller (right hand) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UMotionControllerComponent* R_MotionController;

	/** Motion controller (left hand) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UMotionControllerComponent* L_MotionController;
	
	//Sounds
	UPROPERTY(VisibleAnywhere, Category = "Sound")
		USceneComponent* Sounds = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Sound")
		UAudioComponent* SoundMatch = nullptr; 
public:
	AUnrealProjectCharacter();

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

protected:
	virtual void BeginPlay();

public:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerLight)
		float LightBrightess = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerLight)
		float StartLightBrightess = 100000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerLight)
		float LightDuration = 5.f;

	//Alpha used in Lerp in AnimLight
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerLight)
	float SpeedTransition = 0.5;
	
	/** Whether to use motion controller location for aiming. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	uint32 bUsingMotionControllers : 1;


	bool bLightIsActive = false;
protected:

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	void ActiveLight();
	void DesactiveLight();
	void AnimLight(float DeltaSeconds);
	
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
	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

public:
	/** Returns Mesh1P subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	
	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

private:
	FTimerHandle HitTimeHandle;
	float HitTiming = 1.5f;

	float AlphaLerpLight = 0.0f;
};

