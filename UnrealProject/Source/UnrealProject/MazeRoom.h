// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include <random>

#include "MazeRoom.generated.h"

class AUnrealProjectGameMode;
UCLASS()
class UNREALPROJECT_API UMazeRoom : public UActorComponent
{
	GENERATED_BODY()
	
public:	
	UMazeRoom();
	~UMazeRoom();

	void	SetWalls(AUnrealProjectGameMode* Mode, int* Walls, int Height, int Width, int Index);

	bool	GenerateRoom();
	void	CreateRoom(int x, int y);

protected:
	friend AUnrealProjectGameMode;
	AUnrealProjectGameMode* GameMode = nullptr;

	int* RoomWalls;

	UPROPERTY(VisibleAnywhere)
	int	RoomIndex = -1;

	UPROPERTY(VisibleAnywhere)
	int RoomHeight;
	UPROPERTY(VisibleAnywhere)
	int RoomWidth;

	UPROPERTY(VisibleAnywhere)
	int	CurrentX = -1;
	UPROPERTY(VisibleAnywhere)
	int	CurrentY = -1;

	UPROPERTY(VisibleAnywhere)
	int	DoorCount = 0;

	int ItemCount = 0;

	static std::random_device Rd;
	static std::mt19937 Gen;
};
