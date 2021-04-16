// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ObjectsComposition.generated.h"

UCLASS()
class UNREALPROJECT_API AObjectsComposition : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AObjectsComposition();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USceneComponent* ItemPosition = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanHoldItem = true;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USceneComponent* Root = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UChildActorComponent* Matches = nullptr;
};
