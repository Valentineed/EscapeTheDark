// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Item.h"
#include "GameFramework/GameModeBase.h"
#include "UnrealProjectGameMode.generated.h"

#define NORTH	0
#define SOUTH	1
#define WEST	2
#define EAST	3


class AEndDoor;
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

UENUM()
enum EObjectPlacement
{
	ROOM_MIDDLE,
	ROOM_WALL,
	DEAD_END,
	EMPTY
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

	void	CheckWinConditions(AUnrealProjectCharacter* Player);

	float	GetModuleSize() const { return ModuleSize; }

	UFUNCTION(BlueprintCallable)
	TArray<int>	GetItems() const;

	FVector	GetSpawnLocation() const;

	AEndDoor* EndDoor = nullptr;

	UFUNCTION(BlueprintCallable)
	TArray<TSubclassOf<class ASimpleObject>> GetSimpleObjects();

	UPROPERTY(BlueprintAssignable)
	FOnWin OnWin;

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

	void	GenerateLights();

	void	SetupSpawn();
	bool	IsRoom(int x, int y) const;
	bool	LightNotNear(int x, int y);
	AActor*	PlaceLight(float x, float y) const;
	void	PlaceCorridorSwitch(AActor* Light, int x, int y) const;
	void	PlaceRoomLights();

	void	GenerateItems();
	void	PlaceItem(int x, int y, int Type) const;
	void	PlaceItem(FVector Transform, int Type) const;
	TSubclassOf<AItem>	GetItemType(int index) const;

	void	GenerateObjects();
	void	GenerateDeadEndObject(int x, int y);
	void	GenerateRoomMiddleObject(int x, int y);
	void	GenerateRoomWallObject(int x, int y);
	void	PlaceBigObject(FTransform Transform);
	void	PlaceSmallObject(FTransform Transform);

	void	SpawnEnemy() const;

	int*	Grid;
	int*	RoomsGrid;
	EObjectPlacement*	ObjectsGrid; // eligible objects cases
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

	UPROPERTY(EditAnywhere)
	float	MinEnemySpawnDistanceToPlayer = 12.f;

	/* Enemy */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Subclasses")
	TSubclassOf<class AEnemyCharacter> EnemyType = nullptr;

	/* Spawn */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Subclasses")
	TSubclassOf<class AEndDoor> EndDoorType = nullptr;

	/* Items */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Subclasses")
		TSubclassOf<AItem> Item1Type = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Subclasses")
		TSubclassOf<AItem> Item2Type = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Subclasses")
		TSubclassOf<AItem> Item3Type = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Subclasses")
		TSubclassOf<AItem> Item4Type = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Subclasses")
		TSubclassOf<AItem> Item5Type = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Subclasses")
		TSubclassOf<AItem> Item6Type = nullptr;

	/* Maze modules */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Subclasses")
	TSubclassOf<class AMazeModule> LineModule = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Subclasses")
	TSubclassOf<class AMazeModule> TurnModule = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Subclasses")
	TSubclassOf<class AMazeModule> TModule = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Subclasses")
	TSubclassOf<class AMazeModule> DeadEndModule = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Subclasses")
	TSubclassOf<class AMazeModule> CrossroadsModule = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Subclasses")
	TSubclassOf<class AMazeModule> SimpleWall = nullptr;

	/* Light */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Subclasses")
	TSubclassOf<class ANeon> NeonType = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Subclasses")
	TSubclassOf<class ALightButton> SwitchType = nullptr;

	/* Object */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Subclasses")
	TArray<TSubclassOf<class AObjectsComposition>> ObjectsComposition;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Subclasses")
	TArray<TSubclassOf<class ASimpleObject>> SimpleObjects;

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
