// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjectsComposition.h"

// Sets default values
AObjectsComposition::AObjectsComposition()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(FName("DefaultSceneRoot"));
	RootComponent = Root;

	Matches = CreateDefaultSubobject<UChildActorComponent>(FName("Matches"));
	Matches->SetupAttachment(RootComponent);


	ItemPosition = CreateDefaultSubobject<USceneComponent>(FName("ItemPosition"));
	ItemPosition->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AObjectsComposition::BeginPlay()
{
	Super::BeginPlay();

	SetActorScale3D({ 0.4f, 0.4f, 0.4f });

	if(FMath::RandRange(0, 2) > 0)
	{
		if(Matches != nullptr && Matches->IsValidLowLevel())
		    Matches->DestroyComponent();
	}
}

