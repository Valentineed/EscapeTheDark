// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "UnrealProjectCharacter.generated.h"

class UBoxComponent;
class UPointLightComponent;
class UCameraShake;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameOver);
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

	
	//Sounds
	UPROPERTY(VisibleAnywhere, Category = "Sound")
		USceneComponent* Sounds = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Sound")
		UAudioComponent* SoundMatch = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Sound")
		UAudioComponent* SoundBreathe = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Sound")
		UAudioComponent* SoundWalk = nullptr;
	
	UPROPERTY(VisibleAnywhere, Category = "Sound")
		UAudioComponent* SoundPickUp = nullptr;
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "InteractionBox")
		UBoxComponent* InteractionBox = nullptr;
	AUnrealProjectCharacter();

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

protected:
	virtual void BeginPlay();

	UPROPERTY(BlueprintAssignable)
		FOnGameOver    OnGameOver;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UMG Game")
		TSubclassOf<UUserWidget> StartingWidgetClass;

	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UMG Game")
	UUserWidget* UserInterface = nullptr;
public:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerLight)
		float LightBrightess = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerLight)
		float StartLightBrightess = 100000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerLight)
		float LightDuration = 5.f;

	//Alpha used in Lerp in AnimLight
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerLight, meta =(ClampMin = "0", ClampMax = "1"))
	float SpeedTransition = 0.5;
	
	/** Whether to use motion controller location for aiming. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	uint32 bUsingMotionControllers : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SpeedPlayer)
		bool PlayerLose = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SpeedPlayer)
		float RunSpeed = 1200;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SpeedPlayer)
		float WalkSpeed = 600;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Matches)
		int NumberOfMatches = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Matches)
		int NmbrMatchesToAdd = 10;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Itens)
		TArray<int> Itens;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Keys)
		bool PickItem = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Keys)
		int IndexItem = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Keys)
		bool ChangedValue = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Keys)
		bool BagVisibility = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Shaking)
	TSubclassOf<UCameraShake> CamShake;
	
	bool bLightIsActive = false;
protected:

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	void ActiveLight();
	void DesactiveLight();
	void AnimLight(float DeltaSeconds);

	void ActiveInteractionBox();
	void DesactiveInteractionBox();

	void ShowBag();
	void DisableBag();


	void Run();
	void Walk();
	void ShakeMove();
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
	UFUNCTION()
		virtual void BeginOverlapDetection(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	FTimerHandle HitTimeHandle;
	float HitTiming = 1.5f;

	FTimerHandle HitTimeHandleInteraction;
	
	float AlphaLerpLight = 0.0f;
};

