// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "UnrealProjectGameMode.generated.h"

#define NORTH	0
#define SOUTH	1
#define WEST	2
#define EAST	3


class AUnrealProjectCharacter;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWin);

class LogicItens;
UENUM()
enum EModuleType
{
	GROUND,
	WALL,
	ROOM_GROUND,
	DOOR_UP,
	DOOR_DOWN,
	DOOR_LEFT,
	DOOR_RIGHT
};

class UMazeRoom;
UCLASS(minimalapi)
class AUnrealProjectGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AUnrealProjectGameMode();

	virtual void BeginPlay() override;

    /**
	 * Should be called only once.
	 */
	void	GenerateMaze() const;

	float	GetModuleSize() const { return ModuleSize; }

	UFUNCTION(BlueprintCallable)
	TArray<int>	GetItems() const;

	UPROPERTY(BlueprintAssignable)
	FOnWin OnWin;

	void	CheckWinConditions(AUnrealProjectCharacter* Player);

protected:
	friend	UMazeRoom;

	void	DrawRooms();

	void	CreateModules() const;

	void	ResetGrid() const;
    int		XYToIndex(int x, int y) const;
    int		IsInBounds(int x, int y) const;
    void	Visit(int x, int y) const;

	int		NumberOfWalls(int x, int y, const EModuleType& ModuleType) const;
	void	BreakDeadEndWall(int x, int y) const;
	void	BreakWall(int x, int y) const;

	void	GenerateDoors();
	void	CreateWall(FTransform Transform);

	void	GenerateLights();

	void	SetupSpawn();
	bool	IsRoom(int x, int y);
	bool	LightNotNear(int x, int y);
	AActor*	PlaceLight(float x, float y) const;
	void	PlaceCorridorSwitch(AActor* Light, int x, int y);
	void	PlaceRoomLights();

	void	GenerateItems();

	int*	Grid;
	int*	RoomsGrid;
	int* BonusesGrid;
	TArray<FVector2D>	LightsLocation;

	LogicItens* NeededItems;

    /**
	 * Maze's width measured in number of modules.
	 */
	UPROPERTY(EditAnywhere)
	int	Width = 250;

    /**
	 * Maze's height measured in number of modules.
	 */
	UPROPERTY(EditAnywhere)
	int	Height = 250;

    /**
	 * Maze bottom left corner X coordinate.
	 */
	UPROPERTY(EditAnywhere)
	float	MazeX = 0.f;

	/**
	 * Maze bottom left corner Y coordinate.
	 */
	UPROPERTY(EditAnywhere)
	float	MazeY = 0.f;

    /**
	 * X and Y size of modules.
	 */
	UPROPERTY(EditAnywhere)
	int	ModuleSize = 300;

	/* Spawn */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class AEndDoor> EndDoorType = nullptr;

	/* Maze modules */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class AMazeModule> LineModule = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class AMazeModule> TurnModule = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class AMazeModule> TModule = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class AMazeModule> DeadEndModule = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class AMazeModule> CrossroadsModule = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class AMazeModule> SimpleWall = nullptr;

	/* Light */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class ANeon> NeonType = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class ALightButton> SwitchType = nullptr;

	UPROPERTY(VisibleAnywhere)
	TArray<UMazeRoom*>	Rooms;

    /**
	 * Distance in module between each light in corridors.
	 */
	UPROPERTY(EditAnywhere)
	int	LightCorridorsMinDistance = 6;

	/**
	 * Distance in module between each light in rooms.
	 */
	UPROPERTY(EditAnywhere)
	int	LightRoomsMinDistance = 3;

    /**
	 * Height of the Plafond
	 */
	UPROPERTY(EditAnywhere)
	float	PlafondHeight = 600;

	/**
	 * Height of the Switches
	 */
	UPROPERTY(EditAnywhere)
	float	SwitchHeight = 150;

    /**
	 * Maximum number of Item per room.
	 */
	UPROPERTY(EditAnywhere)
	int		MaxItemPerRoom = 3;
};
