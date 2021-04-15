// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Neon.generated.h"

class UBoxComponent;
UCLASS()
class UNREALPROJECT_API ANeon : public AActor
{
	GENERATED_BODY()

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collider")
		UBoxComponent* Collision = nullptr;

	/** Pawn mesh*/
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		class UStaticMeshComponent* Mesh = nullptr;

	/**/
	UPROPERTY(VisibleDefaultsOnly, Category = Light)
		class USpotLightComponent* Light = nullptr;

	UPROPERTY(VisibleAnywhere)
		class UParticleSystemComponent* Particle;

public:
	// Sets default values for this actor's properties
	ANeon();
	void StartAnimation();

	bool IsVisible();
	void ActiveLight();
	void ActiveLightAnim();
	void AnimationEnd();
	void DesactiveLight();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
		USceneComponent* Sounds = nullptr;

	UPROPERTY(VisibleAnywhere)
		UAudioComponent* SoundAnim = nullptr;

	UPROPERTY(VisibleAnywhere)
		UAudioComponent* SoundExplode = nullptr;


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SettingsLight)
		float AnimDuration = 10.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SettingsLight)
		float MaxTimingToDesactive = 0.8f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SettingsLight)
		float MaxTimingToActive = 2.f;

private:
	void RunAnimation();
	FTimerHandle HitTimeHandle;
	float HitTiming = 1.5f;

	FTimerHandle HitTimeHandleStart;

	FTimerHandle HitTimeHandleEnd;

	bool bAnimation = false;

	bool bCallStop = false;

	bool bActiveLight = true;

	bool btest = false;

	bool bStopAnimation = false;

};
