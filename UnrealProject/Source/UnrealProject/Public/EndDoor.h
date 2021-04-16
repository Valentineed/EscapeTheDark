// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EndDoor.generated.h"

class UInteractiveObject;
class UBoxComponent;
UCLASS()
class UNREALPROJECT_API AEndDoor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEndDoor();

	virtual void BeginPlay() override;

protected:

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* MeshComponent = nullptr;

	UPROPERTY(EditAnywhere)
	UBoxComponent* BoxCollision = nullptr;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "InteractiveObject")
		UInteractiveObject* InterOBJ = nullptr;


	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};
