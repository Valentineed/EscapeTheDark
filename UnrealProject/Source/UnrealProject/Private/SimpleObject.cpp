// Fill out your copyright notice in the Description page of Project Settings.


#include "SimpleObject.h"

// Sets default values
ASimpleObject::ASimpleObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(FName("DefaultSceneRoot"));
	RootComponent = StaticMeshComponent;
}
