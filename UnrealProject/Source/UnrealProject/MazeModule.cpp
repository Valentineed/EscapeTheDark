// Fill out your copyright notice in the Description page of Project Settings.


#include "MazeModule.h"
#include "UnrealProjectGameMode.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AMazeModule::AMazeModule()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(FName("Mesh"));

	RootComponent = MeshComponent;
}

// Called when the game starts or when spawned
void AMazeModule::BeginPlay()
{
	Super::BeginPlay();

	AUnrealProjectGameMode* GameMode = Cast<AUnrealProjectGameMode>(UGameplayStatics::GetGameMode(this));

	if (GameMode)
	{
		const float ModuleScale = GameMode->GetModuleSize() / 100;

		MeshComponent->SetRelativeScale3D({ ModuleScale, ModuleScale, ModuleScale });
		AddActorLocalOffset({ 0, 0, -50 });
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Could not find right gamemode to set module size"));
	}
}

