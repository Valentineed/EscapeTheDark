// Copyright Epic Games, Inc. All Rights Reserved.

#include "UnrealProjectCharacter.h"

#include "InteractiveObject.h"
#include "Item.h"
#include "Matches.h"
#include "UnrealProjectProjectile.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "UnrealProjectGameMode.h"
#include "Blueprint/UserWidget.h"
#include "Components/AudioComponent.h"
#include "Components/BoxComponent.h"
#include "Components/PointLightComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AUnrealProjectCharacter

AUnrealProjectCharacter::AUnrealProjectCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	//CreateLight
	Light = CreateDefaultSubobject<UPointLightComponent>(TEXT("PlayerLight"));
	if(Light)
	{
		Light->SetupAttachment(FirstPersonCameraComponent);
		Light->SetIntensity(0);
	}

	InteractionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractiveBox"));
	if(InteractionBox)
	{
		InteractionBox->SetupAttachment(RootComponent);
		InteractionBox->SetCollisionProfileName(TEXT("Trigger"));
		InteractionBox->OnComponentBeginOverlap.AddDynamic(this, &AUnrealProjectCharacter::BeginOverlapDetection);
	}
	
	//CreateSound
	Sounds = CreateDefaultSubobject<USceneComponent>(FName("Sounds"));
	SoundMatch = CreateDefaultSubobject<UAudioComponent>(FName(TEXT("MatchSound")));
	SoundBreathe = CreateDefaultSubobject<UAudioComponent>(FName(TEXT("BreatheSound")));
	SoundWalk = CreateDefaultSubobject<UAudioComponent>(FName(TEXT("WalkSound")));
	SoundPickUp = CreateDefaultSubobject<UAudioComponent>(FName(TEXT("PickUpSound")));
	SoundRespirationWalk = CreateDefaultSubobject<UAudioComponent>(FName(TEXT("RespirationWalkSound")));
	if (Sounds != nullptr)
	{
		Sounds->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
		if(SoundMatch)
		{
			SoundMatch->SetupAttachment(Sounds);
			SoundMatch->bAutoActivate = false;
		}
		if(SoundBreathe)
		{
			SoundBreathe->SetupAttachment(Sounds);
			SoundBreathe->bAutoActivate = false;
		}
		if(SoundWalk)
		{
			SoundWalk->SetupAttachment(Sounds);
			SoundWalk->bAutoActivate = false;
		}
		if (SoundPickUp)
		{
			SoundPickUp->SetupAttachment(Sounds);
			SoundPickUp->bAutoActivate = false;
		}
		if (SoundRespirationWalk)
		{
			SoundRespirationWalk->SetupAttachment(Sounds);
			SoundRespirationWalk->bAutoActivate = false;
		}
	}	
}

void AUnrealProjectCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if(bLightIsActive)
	{
		AnimLight(DeltaSeconds);
	}

	ShakeMove();
}

void AUnrealProjectCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

//////////////////////////////////////////////////////////////////////////
// Input

void AUnrealProjectCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	//Bind light of player
	PlayerInputComponent->BindAction("ActiveLight", IE_Pressed, this, &AUnrealProjectCharacter::ActiveLight);

	PlayerInputComponent->BindAction("Interaction", IE_Pressed, this, &AUnrealProjectCharacter::ActiveInteractionBox);

	PlayerInputComponent->BindAction("Bag", IE_Pressed, this, &AUnrealProjectCharacter::ShowBag);
	PlayerInputComponent->BindAction("Bag", IE_Released, this, &AUnrealProjectCharacter::DisableBag);

	
	PlayerInputComponent->BindAction("Run", IE_Pressed, this, &AUnrealProjectCharacter::Run);
	PlayerInputComponent->BindAction("Run", IE_Released, this, &AUnrealProjectCharacter::Walk);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &AUnrealProjectCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AUnrealProjectCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AUnrealProjectCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AUnrealProjectCharacter::LookUpAtRate);
}

void AUnrealProjectCharacter::BeginOverlapDetection(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

void AUnrealProjectCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AUnrealProjectCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AUnrealProjectCharacter::ActiveLight()
{
	if(bLightIsActive == false && NumberOfMatches > 0)
	{
		NumberOfMatches--;
		if(!SoundMatch->IsPlaying())
		{
			SoundMatch->Activate(true);
		}
		bLightIsActive = true;
		GetWorldTimerManager().SetTimer(HitTimeHandle, this, &AUnrealProjectCharacter::DesactiveLight, LightDuration);
	}
}

void AUnrealProjectCharacter::DesactiveLight()
{
	AlphaLerpLight = 0.0f;
	Light->SetIntensity(0.f);
	bLightIsActive = false;
}

void AUnrealProjectCharacter::AnimLight(float DeltaSeconds)
{
	AlphaLerpLight += SpeedTransition * DeltaSeconds;
	AlphaLerpLight = FMath::Clamp(AlphaLerpLight, 0.f, 1.f);
	float intensity = FMath::Lerp(StartLightBrightess, LightBrightess, AlphaLerpLight);
	Light->SetIntensity(intensity);
}

void AUnrealProjectCharacter::ActiveInteractionBox()
{
	TArray<AActor*>actors;
	InteractionBox->GetOverlappingActors(actors);
    for(auto*  actor : actors) 
	{
		UInteractiveObject* component = actor->FindComponentByClass<UInteractiveObject>();
		if(component) 
		{
			component->bIsSelected = true;
			auto* item = Cast<AItem>(actor);
			if(item)
			{
				Itens.Add(item->Index);
				PickItem = true;
				IndexItem = item->Index;
				item->Destroy();
				SoundPickUp->Play();
			}
			else
			{
			  auto* match = Cast<AMatches>(actor);
			  if(match)
			  {
				  NumberOfMatches += NmbrMatchesToAdd;
				  match->Destroy();
				  SoundPickUp->Play();
			  }
			}
		}
    }
}

void AUnrealProjectCharacter::DesactiveInteractionBox()
{
	InteractionBox->SetCollisionProfileName(TEXT("NoCollision"));
}

void AUnrealProjectCharacter::ShowBag()
{
	ChangedValue = true;
	BagVisibility = true;
}

void AUnrealProjectCharacter::DisableBag()
{
	//DisableBagBP();
	ChangedValue = true;
	BagVisibility = false;
}

void AUnrealProjectCharacter::Run()
{
	GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
}

void AUnrealProjectCharacter::Walk()
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void AUnrealProjectCharacter::ShakeMove()
{
	if (GetVelocity().Size() > 0.0f && CanJump())
	{
		if (SoundBreathe->IsPlaying())
		{
			SoundBreathe->Stop();
		}
		if (!SoundWalk->IsPlaying())
		{
			SoundWalk->Play();
		}
		if (!SoundRespirationWalk->IsPlaying())
		{
			SoundRespirationWalk->Play();
		}
		GetWorld()->GetFirstPlayerController()->PlayerCameraManager->PlayCameraShake(CamShake);
	}
	else
	{
		if(!SoundBreathe->IsPlaying())
		{
			SoundBreathe->Play();
		}
		if (SoundWalk->IsPlaying())
		{
			SoundWalk->Stop();
		}
		if (SoundWalk->IsPlaying())
		{
			SoundWalk->Stop();
		}
	}
}

void AUnrealProjectCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AUnrealProjectCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}
