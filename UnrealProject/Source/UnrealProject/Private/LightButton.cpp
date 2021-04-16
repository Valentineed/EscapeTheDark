// Fill out your copyright notice in the Description page of Project Settings.


#include "LightButton.h"

#include "Neon.h"
#include "Components/AudioComponent.h"
#include "Components/BoxComponent.h"
#include "UnrealProject/Public/InteractiveObject.h"


// Sets default values
ALightButton::ALightButton()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collider"));
	if(Collision)
	{
		SetRootComponent(Collision);
	}
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	if(Mesh)
	{
		Mesh->SetupAttachment(RootComponent);
	}
	InterOBJ = CreateDefaultSubobject<UInteractiveObject>(TEXT("InteractiveOBJ"));
	
	Sounds = CreateDefaultSubobject<USceneComponent>(FName("Sounds"));
	SoundActiveButton = CreateDefaultSubobject<UAudioComponent>(FName(TEXT("MatchSound")));
	if (Sounds != nullptr)
	{
		Sounds->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
		if (SoundActiveButton)
		{
			SoundActiveButton->SetupAttachment(Sounds);
			SoundActiveButton->bAutoActivate = false;
		}
	}
}

// Called when the game starts or when spawned
void ALightButton::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ALightButton::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

        if(InterOBJ->bIsSelected)
        {
		ActiveLight();
		InterOBJ->bIsSelected = false;
        }
}

void ALightButton::ActiveLight()
{
	SoundActiveButton->Play();

	for(ANeon* CurrentNeon : Neon)
	{
		if (CurrentNeon->IsVisible() == false)
		{
			CurrentNeon->ActiveLight();
		}
		else
		{
			CurrentNeon->DesactiveLight();
		}
	    
	}
}

void ALightButton::AddNeon(ANeon* TheNeon)
{
	if(TheNeon)
	{
		Neon.Emplace(TheNeon);
	}
}

