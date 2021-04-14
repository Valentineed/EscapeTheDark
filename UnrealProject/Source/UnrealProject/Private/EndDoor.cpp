// Fill out your copyright notice in the Description page of Project Settings.


#include "EndDoor.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UnrealProject/UnrealProjectCharacter.h"
#include "UnrealProject/UnrealProjectGameMode.h"

// Sets default values
AEndDoor::AEndDoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(FName("Mesh"));

	RootComponent = MeshComponent;

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(FName("BoxCollision"));
	BoxCollision->SetupAttachment(RootComponent);
}

void AEndDoor::BeginPlay()
{
	Super::BeginPlay();
	
	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &AEndDoor::OnBeginOverlap);
}

void AEndDoor::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(AUnrealProjectCharacter* Player = Cast<AUnrealProjectCharacter>(OtherActor))
	{
		AUnrealProjectGameMode* GameMode = Cast<AUnrealProjectGameMode>(UGameplayStatics::GetGameMode(this));

		if(GameMode)
		{
			GameMode->CheckWinConditions(Player);
		}
	}
}
