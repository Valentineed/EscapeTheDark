// Copyright Epic Games, Inc. All Rights Reserved.

#include "UnrealProjectGameMode.h"


#include <ctime>
#include <string>

#include "ObjectsComposition.h"
#include "SimpleObject.h"
#include "EnemyCharacter.h"
#include "EndDoor.h"
#include "LogicItens.h"
#include "LightButton.h"
#include "MazeModule.h"
#include "Neon.h"
#include "MazeRoom.h"
#include "UnrealProjectCharacter.h"
#include "UnrealProjectHUD.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

AUnrealProjectGameMode::AUnrealProjectGameMode() : Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AUnrealProjectHUD::StaticClass();

    LineModule = AMazeModule::StaticClass();
    CrossroadsModule = AMazeModule::StaticClass();
    TModule = AMazeModule::StaticClass();

    Grid = new int[Width * Height];
    RoomsGrid = new int[Width * Height];
    BonusesGrid = new int[Width * Height];
    ObjectsGrid = new EObjectPlacement[Width * Height];

    NeededItems = new LogicItens;

    Rooms.Reserve(5);
    Rooms.Add(CreateDefaultSubobject<UMazeRoom>(FName("Room0")));
    Rooms.Add(CreateDefaultSubobject<UMazeRoom>(FName("Room1")));
    Rooms.Add(CreateDefaultSubobject<UMazeRoom>(FName("Room2")));
    Rooms.Add(CreateDefaultSubobject<UMazeRoom>(FName("Room3")));
    Rooms.Add(CreateDefaultSubobject<UMazeRoom>(FName("Room4")));
}

void AUnrealProjectGameMode::BeginPlay()
{
    Super::BeginPlay();

    DrawRooms();

    GenerateMaze();

    auto TempRooms = Rooms;
    for(int i = TempRooms.Num() - 1; i >= 0; i--)
    {
        UE_LOG(LogTemp, Warning, TEXT("Generating Room"));
        if(!TempRooms[i]->GenerateRoom())
        {
            UE_LOG(LogTemp, Warning, TEXT("Unable to place Room"));
            TempRooms.RemoveAt(i);
        }
    }
    Rooms = TempRooms;

    GenerateDoors();

    CreateModules();

    GenerateItems();

    GenerateLights();

    GenerateObjects();

    SetupSpawn();

    SpawnEnemy();
}

void AUnrealProjectGameMode::GenerateMaze() const
{
    srand(time(nullptr));
    ResetGrid();
    Visit(1, 1);
}

TArray<int> AUnrealProjectGameMode::GetItems() const
{
    return NeededItems->ItensConditionWin;
}

void AUnrealProjectGameMode::DrawRooms()
{
    int Room1[128]{ 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 , 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 , 2, 2, 2, 2, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 , 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 , 2, 2, 2, 2, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 , 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 , 2, 2, 2, 2, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 , 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 , 2, 2, 2, 2 };
    Rooms[0]->SetWalls(this, Room1, 4, 8, 0);

    int Room2[32]{ 0, 0, 2, 2, 2, 2, 2, 2,
                    2, 2, 2, 2, 2, 2, 2, 2,
                    2, 2, 2, 2, 2, 2, 2, 2,
                    2, 2, 2, 2 ,2, 2, 2, 2 };
    Rooms[1]->SetWalls(this, Room2, 4, 8, 0);

    int Room3[32]{ 0, 0, 2, 2, 2, 2, 2, 2,
                    2, 2, 2, 2, 2, 2, 2, 2,
                    2, 2, 2, 2, 2, 2, 2, 2,
                    2, 2, 2, 2 ,2, 2, 2, 2 };
    Rooms[2]->SetWalls(this, Room3, 4, 8, 0);

    int Room4[32]{ 0, 0, 2, 2, 2, 2, 2, 2,
                    2, 2, 2, 2, 2, 2, 2, 2,
                    2, 2, 2, 2, 2, 2, 2, 2,
                    2, 2, 2, 2 ,2, 2, 2, 2 };
    Rooms[3]->SetWalls(this, Room4, 1, 1, 0);

    int Room5[32]{ 0, 0, 2, 2, 2, 2, 2, 2,
                    2, 2, 2, 2, 2, 2, 2, 2,
                    2, 2, 2, 2, 2, 2, 2, 2,
                    2, 2, 2, 2 ,2, 2, 2, 2 };
    Rooms[4]->SetWalls(this, Room5, 1, 1, 0);
}

void AUnrealProjectGameMode::CreateModules() const
{
    for(int iHeight = 0; iHeight < Height; iHeight++)
    {
        for(int jWidth = 0; jWidth < Width; jWidth++)
        {
            if (Grid[XYToIndex(jWidth, iHeight)] == GROUND)
            {
                UClass* ModuleClass = nullptr;
                FRotator ModuleRotation = FRotator::ZeroRotator;
                const int WallsCount = NumberOfWalls(jWidth, iHeight, GROUND);

                if (WallsCount == 2)
                {
                    if (Grid[XYToIndex(jWidth, iHeight - 1)] == Grid[XYToIndex(jWidth, iHeight + 1)]
                        || Grid[XYToIndex(jWidth - 1, iHeight)] == Grid[XYToIndex(jWidth + 1, iHeight)] ) // Line
                    {
                        if (Grid[XYToIndex(jWidth, iHeight + 1)] == GROUND)
                        {
                            ModuleRotation.Yaw = 90;
                        }

                        ModuleClass = LineModule;
                    }
                    else // Turn
                    {
                        if (Grid[XYToIndex(jWidth, iHeight + 1)] != GROUND)
                        {
                            if (Grid[XYToIndex(jWidth + 1, iHeight)] != GROUND)
                            {
                                ModuleRotation.Yaw = 90;
                            }
                            else
                            {
                                ModuleRotation.Yaw = 180;
                            }
                        }
                        else
                        {
                            if (Grid[XYToIndex(jWidth + 1, iHeight)] != GROUND)
                            {
                                ModuleRotation.Yaw = 0;
                            }
                            else
                            {
                                ModuleRotation.Yaw = -90;
                            }
                        }
                        ModuleClass = TurnModule;
                    }
                }
                else if (WallsCount == 1) // T
                {
                    if (Grid[XYToIndex(jWidth, iHeight - 1)] != GROUND)
                    {
                        ModuleRotation.Yaw = -90;
                    }
                    else if (Grid[XYToIndex(jWidth, iHeight + 1)] != GROUND)
                    {
                        ModuleRotation.Yaw = 90;
                    }
                    else if(Grid[XYToIndex(jWidth - 1, iHeight)] != GROUND)
                    {
                        ModuleRotation.Yaw = 180;
                    }

                    ModuleClass = TModule;
                }
                else if (WallsCount == 3) // Dead end
                {
                    if (Grid[XYToIndex(jWidth, iHeight - 1)] == GROUND)
                    {
                        ModuleRotation.Yaw = 180;
                    }
                    else if (Grid[XYToIndex(jWidth, iHeight + 1)] == GROUND)
                    {
                        ModuleRotation.Yaw = 0;
                    }
                    else if (Grid[XYToIndex(jWidth - 1, iHeight)] == GROUND)
                    {
                        ModuleRotation.Yaw = 90;
                    }
                    else
                    {
                        ModuleRotation.Yaw = -90;
                    }
                    ObjectsGrid[XYToIndex(jWidth, iHeight - 1)] = DEAD_END;
                    ModuleClass = DeadEndModule;
                }
                else if (WallsCount == 0) // Crossroad
                {
                    ModuleClass = CrossroadsModule;

                    if(RoomsGrid[XYToIndex(jWidth, iHeight)] != 0) // Room
                    {
                        ObjectsGrid[XYToIndex(jWidth, iHeight)] = ROOM_MIDDLE;
                    }
                }

                if(ModuleClass != nullptr)
                {
                    FActorSpawnParameters SpawnParameters;
                    SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

                    const FTransform Transform(ModuleRotation, FVector{ MazeX + jWidth * ModuleSize, MazeY + iHeight * ModuleSize, 0 });

                    GetWorld()->SpawnActor(ModuleClass, &Transform, SpawnParameters);
                }
            }
        }
    }
}

void AUnrealProjectGameMode::ResetGrid() const
{
    // Fills the grid with walls.
    for (int i=0; i < Width * Height; ++i)
    {
        Grid[i] = WALL;
        RoomsGrid[i] = -1;
        BonusesGrid[i] = 0;
        ObjectsGrid[i] = EMPTY;
    }
}

int AUnrealProjectGameMode::XYToIndex(int x, int y) const
{
    return y * Width + x;
}

int AUnrealProjectGameMode::IsInBounds(int x, int y) const
{
    if (x < 0 || x >= Width)
        return false;

    if (y < 0 || y >= Height)
        return false;

    return true;
}

void AUnrealProjectGameMode::Visit(int x, int y) const
{
    // Starting at the given index, recursively visits every direction in a
    // randomized order.
    // Set my current location to be an empty passage.
    Grid[ XYToIndex(x,y) ] = 0;

    // Create an local array containing the 4 directions and shuffle their order.
    int dirs[4];
    dirs[0] = NORTH;
    dirs[1] = EAST;
    dirs[2] = SOUTH;
    dirs[3] = WEST;
    for (int i=0; i<4; ++i)
    {
        const int r = rand() & 3;
        const int temp = dirs[r];
        dirs[r]  = dirs[i];
        dirs[i]  = temp;
    }
    // Loop through every direction and attempt to Visit that direction.
    for (int i = 0; i < 4; ++i)
    {
        // dx,dy are offsets from current location.  Set them based
        // on the next direction I wish to try.
        int dx = 0, dy = 0;
        switch (dirs[i])
        {
        case NORTH:
            dy = -1;
            break;
        case SOUTH:
            dy = 1;
            break;
        case EAST:
            dx = 1;
            break;

        case WEST:
            dx = -1;
            break;
        default:
            break;
        }

        // Find the (x,y) coordinates of the grid cell 2 spots
        // away in the given direction.
        const int x2 = x + (dx << 1);
        const int y2 = y + (dy << 1);
        if (IsInBounds(x2, y2))
        {
            if (Grid[XYToIndex(x2, y2)] == WALL)
            {
                // (x2,y2) has not been visited yet...knock down the
                // wall between my current position and that position
                Grid[XYToIndex(x2 - dx, y2 - dy)] = GROUND;
                // Recursively Visit (x2,y2)
                Visit(x2, y2);
            }
        }
    }

    for(int i = 1; i < Height-1; i+=2)
    {
        for(int j = 1; j < Width-1; j+=2)
        {
            if(NumberOfWalls(i, j, GROUND) == 3)
            {
                BreakDeadEndWall(j, i);
            }
        }
    }
}

int AUnrealProjectGameMode::NumberOfWalls(const int x, const int y, const EModuleType& ModuleType) const
{
    int WallsCount = 0;

    if(x - 1 < 0 || Grid[XYToIndex(x - 1, y)] != ModuleType)
    {
        WallsCount++;
    }

    if (x + 1 > Width || Grid[XYToIndex(x + 1, y)] != ModuleType)
    {
        WallsCount++;
    }

    if (y - 1 < 0 || Grid[XYToIndex(x, y - 1)] != ModuleType)
    {
        WallsCount++;
    }

    if (y + 1 > Height || Grid[XYToIndex(x, y + 1)] != ModuleType)
    {
        WallsCount++;
    }

    return WallsCount;
}

void AUnrealProjectGameMode::BreakDeadEndWall(const int x, const int y) const
{
    if (Grid[XYToIndex(x - 1, y)] == GROUND)
    {
        BreakWall(x + 1, y);
    }
    else if (Grid[XYToIndex(x + 1, y)] == GROUND)
    {
        BreakWall(x - 1, y);
    }
    else if (Grid[XYToIndex(x, y - 1)] == GROUND)
    {
        BreakWall(x, y + 1);
    }
    else if (Grid[XYToIndex(x, y + 1)] == GROUND)
    {
        BreakWall(x, y - 1);
    }
}

void AUnrealProjectGameMode::BreakWall(const int x, const int y) const
{
    if(x < 0 || y < 0 || x >= Width || y >= Height)
    {
        return;
    }

    Grid[XYToIndex(x, y)] = GROUND;
}

void AUnrealProjectGameMode::GenerateLights()
{
    for(int iHeight = 0; iHeight < Height; iHeight++)
    {
        for(int jWidth = 0; jWidth < Width; jWidth++)
        {
            if(Grid[XYToIndex(jWidth, iHeight)] == GROUND)
            {
                if(!IsRoom(jWidth, iHeight) && NumberOfWalls(jWidth, iHeight, GROUND) > 0)
                {
                    if(LightNotNear(jWidth, iHeight))
                    {
                        PlaceCorridorSwitch(PlaceLight(jWidth, iHeight), jWidth, iHeight);
                        LightsLocation.Add({ static_cast<float>(jWidth), static_cast<float>(iHeight) });
                    }
                }
            }
        }
    }

    PlaceRoomLights();
}

void AUnrealProjectGameMode::SetupSpawn()
{
    UMazeRoom* SpawnRoom = Rooms[0];

    FActorSpawnParameters SpawnParameters;
    SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    FRotator DoorRotation = FRotator::ZeroRotator;

    const int DoorX = SpawnRoom->CurrentX + SpawnRoom->RoomWidth / 2;
    const int DoorY = SpawnRoom->CurrentY + SpawnRoom->RoomHeight / 2;

    FTransform Transform(DoorRotation, FVector{ MazeX + DoorX * ModuleSize, MazeY + DoorY * ModuleSize, 0 });

    bool bDoorPlaced = false;
    for (int j = 0; j < SpawnRoom->RoomHeight; j++)
    {
        if (Grid[XYToIndex(SpawnRoom->CurrentX + SpawnRoom->RoomWidth, SpawnRoom->CurrentY + j)] == WALL) // Valid door
        {
            ObjectsGrid[XYToIndex(SpawnRoom->CurrentX + SpawnRoom->RoomWidth - 1, SpawnRoom->CurrentY + j)] = EMPTY;

            Transform = FTransform(DoorRotation, FVector{ MazeX + (SpawnRoom->CurrentX + SpawnRoom->RoomWidth - 0.5f) * ModuleSize, MazeY + (SpawnRoom->CurrentY + j) * ModuleSize, 0 });

            bDoorPlaced = true;
            break;
        }
    }

    if(!bDoorPlaced)
    for (int j = 0; j < SpawnRoom->RoomHeight; j++)
    {
        if (Grid[XYToIndex(SpawnRoom->CurrentX - 1, SpawnRoom->CurrentY + j)] == WALL)
        {
            ObjectsGrid[XYToIndex(SpawnRoom->CurrentX, SpawnRoom->CurrentY + j)] = EMPTY;

            DoorRotation.Yaw = 180;
            Transform = FTransform(DoorRotation, FVector{ MazeX + (SpawnRoom->CurrentX - 0.5f) * ModuleSize, MazeY + (SpawnRoom->CurrentY + j) * ModuleSize, 0 });
            bDoorPlaced = true;
            break;
        }
    }


    if (!bDoorPlaced)
    for (int j = 0; j < SpawnRoom->RoomWidth; j++) 
    {
        if (Grid[XYToIndex(SpawnRoom->CurrentX + j, SpawnRoom->CurrentY - 1)] == WALL) // Valid door
        {
            ObjectsGrid[XYToIndex(SpawnRoom->CurrentX + j, SpawnRoom->CurrentY)] = EMPTY;

            DoorRotation.Yaw = -90;
            Transform = FTransform(DoorRotation, FVector{ MazeX + (SpawnRoom->CurrentX + j) * ModuleSize, MazeY + (SpawnRoom->CurrentY - 0.5f) * ModuleSize, 0 });
            bDoorPlaced = true;
            break;
        }
    }

    if (!bDoorPlaced)
    for (int j = 0; j < SpawnRoom->RoomWidth; j++) 
    {
        if (Grid[XYToIndex(SpawnRoom->CurrentX + j, SpawnRoom->CurrentY + SpawnRoom->RoomHeight)] == WALL) // Valid door
        {
            ObjectsGrid[XYToIndex(SpawnRoom->CurrentX + j, SpawnRoom->CurrentY)] = EMPTY;

            DoorRotation.Yaw = 90;
            Transform = FTransform(DoorRotation, FVector{ MazeX + (SpawnRoom->CurrentX + j) * ModuleSize, MazeY + (SpawnRoom->CurrentY + SpawnRoom->RoomHeight - 0.5f) * ModuleSize, 0 });
            bDoorPlaced = true;
            break;
        }
    }


    EndDoor = Cast<AEndDoor>(GetWorld()->SpawnActor(EndDoorType, &Transform, SpawnParameters));
}

void AUnrealProjectGameMode::GenerateDoors()
{

    for(const auto& Room : Rooms)
    {
        //int DoorCount = 0;
        //if(Room->CurrentX > 0)
        //{
        //    bool bDoorFound = false;
        //    for (int j = 0; j < Room->RoomHeight; j++) // Left wall
        //    {
        //        if (Grid[XYToIndex(Room->CurrentX - 1, Room->CurrentY + j)] == GROUND) // Valid door
        //        {
        //            if (bDoorFound)
        //            {

        //                UE_LOG(LogTemp, Warning, TEXT("Creating Wall"));
        //                CreateWall(FTransform(FRotator(0, -90, 0), { MazeX + Room->CurrentX * ModuleSize, MazeY + (Room->CurrentY + j) * ModuleSize, 0 }));
        //            }
        //            else
        //            {
        //                RoomsGrid[XYToIndex(Room->CurrentX, Room->CurrentY + j)] = DOOR_LEFT;

        //                DoorCount++;
        //                bDoorFound = true;
        //            }
        //        }
        //    }
        //}

        //for (int j = 0; j < Room->RoomHeight; j++) // Right wall
        //{
        //    bool bDoorFound = false;
        //    const int RoomUpX = Room->CurrentX + Room->RoomWidth;
        //    if (Grid[XYToIndex(RoomUpX + 1, Room->CurrentY + j)] == GROUND) // Valid door
        //    {
        //        RoomsGrid[XYToIndex(RoomUpX , Room->CurrentY + j)] = DOOR_RIGHT;

        //        DoorCount++;
        //        bDoorFound = true;
        //    }

        //    if (bDoorFound)
        //        break;
        //}

        //for (int j = 0; j < Room->RoomWidth; j++) // Down wall
        //{
        //    bool bDoorFound = false;
        //    if (Grid[XYToIndex(Room->CurrentX + j, Room->CurrentY - 1)] == GROUND) // Valid door
        //    {
        //        RoomsGrid[XYToIndex(Room->CurrentX + j, Room->CurrentY )] = DOOR_DOWN;

        //        DoorCount++;
        //        bDoorFound = true;
        //    }

        //    if (bDoorFound)
        //        break;
        //}

        //for (int j = 0; j < Room->RoomWidth; j++) // Up wall
        //{
        //    bool bDoorFound = false;
        //    const int RoomUpY = Room->CurrentY + Room->RoomHeight;

        //    if (Grid[XYToIndex(Room->CurrentX + j, RoomUpY + 1)] == GROUND) // Valid door
        //    {
        //        RoomsGrid[XYToIndex(Room->CurrentX + j, RoomUpY)] = DOOR_UP;

        //        DoorCount++;
        //        bDoorFound = true;
        //    }

        //    if (bDoorFound)
        //        break;
        //}

        //Room->DoorCount = DoorCount;

        //if(DoorCount == 0)
        //{
        //    UE_LOG(LogTemp, Error, TEXT("A Room was created without a door"));
        //}
    }
}

void AUnrealProjectGameMode::CreateWall(FTransform Transform) const
{
    FActorSpawnParameters SpawnParameters;
    SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    GetWorld()->SpawnActor(SimpleWall, &Transform, SpawnParameters);
}

bool AUnrealProjectGameMode::IsRoom(int x, int y) const
{
    if (Grid[XYToIndex(x, y)] != GROUND)
        return false;

    if(RoomsGrid[XYToIndex(x, y)] > DOOR_UP)
    {
        return true;
    }

    return false;
}

bool AUnrealProjectGameMode::LightNotNear(int x, int y)
{
    for(const auto& Light : LightsLocation)
    {
        if(FVector2D::Distance(Light, {static_cast<float>(x), static_cast<float>(y)}) < LightCorridorsMinDistance)
        {
            return false;
        }
    }
    return true;
}

AActor* AUnrealProjectGameMode::PlaceLight(float x, float y) const
{
    FActorSpawnParameters SpawnParameters;
    SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    const FTransform Transform(FRotator::ZeroRotator, FVector{ (float)MazeX + x * (float)ModuleSize, (float)MazeY + y * (float)ModuleSize, PlafondHeight });

    return GetWorld()->SpawnActor(NeonType, &Transform, SpawnParameters);
}

void AUnrealProjectGameMode::PlaceCorridorSwitch(AActor* Light, int x, int y) const
{
    if(NumberOfWalls(x, y, GROUND) == 0)
    {
        return;
    }

    int SafeIncrement = 0;
    while(SafeIncrement < 10)
    {
        /* Random switch wall placement*/
        const bool bIncrementRatio = FMath::RandBool();
        const bool bIncrementX = FMath::RandBool();

        const int IncrementRatio = bIncrementRatio ? 1 : -1;

        FVector2D WallCoordinates{ 0, 0 };

        if(bIncrementX)
        {
            WallCoordinates.X = IncrementRatio;
        }
        else
        {
            WallCoordinates.Y = IncrementRatio;
        }

        if(Grid[XYToIndex(x + WallCoordinates.X, y + WallCoordinates.Y)] == WALL)
        {
            FActorSpawnParameters SpawnParameters;
            SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

            FRotator Rotation = FRotator::ZeroRotator;

            if(bIncrementX)
            {
                if(bIncrementRatio)
                {
                    Rotation.Yaw = 90;
                }
                else
                {
                    
                }
            }
            else
            {
                if (bIncrementRatio)
                {

                }
                else
                {

                }
            }

            const FTransform Transform(Rotation, FVector{ MazeX + x * ModuleSize + WallCoordinates.X * ModuleSize / 2, MazeY + y * ModuleSize + WallCoordinates.Y * ModuleSize / 2, SwitchHeight });

            auto* Switch = Cast<ALightButton>(GetWorld()->SpawnActor(SwitchType, &Transform, SpawnParameters));

            if(Switch)
            {
                Switch->AddNeon(Cast<ANeon>(Light));
                return;
            }
        }
        SafeIncrement++;
    }

}

void AUnrealProjectGameMode::PlaceRoomLights()
{
    for(const auto& Room : Rooms)
    {
        TArray<ANeon*> Lights;
        TArray<ALightButton*> Switches;

        if(Room->RoomHeight < Room->RoomWidth)
        {
            const int LightCount = (float)Room->RoomWidth / (float)LightRoomsMinDistance;
            const float LightsDistance = static_cast<float>(Room->RoomWidth) / (static_cast<float>(LightCount) + 1.f);

            for(int i = 0; i < LightCount; i++)
            {
                Lights.Add(Cast<ANeon>(PlaceLight((float)Room->CurrentX + LightsDistance * ((float)i + 1.f), (float)Room->CurrentY + (float)(Room->RoomHeight - 1) / 2.f)));
            }
        }
        else
        {
            const int LightCount = (float)Room->RoomHeight / (float)LightRoomsMinDistance;
            const float LightsDistance = static_cast<float>(Room->RoomHeight) / (static_cast<float>(LightCount) + 1.f);

            for (int i = 0; i < LightCount; i++)
            {
                Lights.Add(Cast<ANeon>(PlaceLight((float)Room->CurrentX + (float)(Room->RoomWidth - 1) / 2.f, (float)Room->CurrentY + LightsDistance * ((float)i + 1.f))));
            }
        }

        FActorSpawnParameters SpawnParameters;
        SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        FRotator Rotation = FRotator::ZeroRotator;

        if(Room->CurrentX > 0)
        {
            for (int j = 0; j < Room->RoomHeight; j++) // Left wall
            {
                if (Grid[XYToIndex(Room->CurrentX - 1, Room->CurrentY + j)] == WALL) // Valid door
                {
                    const FTransform Transform(Rotation, FVector{ MazeX + (Room->CurrentX - 0.5f) * ModuleSize, MazeY + (Room->CurrentY + j) * ModuleSize, SwitchHeight });

                    Switches.Add(Cast<ALightButton>(GetWorld()->SpawnActor(SwitchType, &Transform, SpawnParameters)));

                    break;
                }
            }
        }

        for (int j = 0; j < Room->RoomHeight; j++) // Left wall
        {
            if (Grid[XYToIndex(Room->CurrentX + Room->RoomWidth, Room->CurrentY + j)] == WALL) // Valid door
            {
                const FTransform Transform(Rotation, FVector{ MazeX + (Room->CurrentX + Room->RoomWidth - 0.5f) * ModuleSize, MazeY + (Room->CurrentY + j) * ModuleSize, SwitchHeight });

                Switches.Add(Cast<ALightButton>(GetWorld()->SpawnActor(SwitchType, &Transform, SpawnParameters)));

                break;
            }
        }

        for (int j = 0; j < Room->RoomWidth; j++) // Left wall
        {
            if (Grid[XYToIndex(Room->CurrentX + j, Room->CurrentY - 1)] == WALL) // Valid door
            {
                const FTransform Transform(Rotation, FVector{ MazeX + (Room->CurrentX + j) * ModuleSize, MazeY + (Room->CurrentY - 0.5f) * ModuleSize, SwitchHeight });

                Switches.Add(Cast<ALightButton>(GetWorld()->SpawnActor(SwitchType, &Transform, SpawnParameters)));

                break;
            }
        }

        for (int j = 0; j < Room->RoomWidth; j++) // Left wall
        {
            if (Grid[XYToIndex(Room->CurrentX + j, Room->CurrentY + Room->RoomHeight)] == WALL) // Valid door
            {
                const FTransform Transform(Rotation, FVector{ MazeX + (Room->CurrentX + j) * ModuleSize, MazeY + (Room->CurrentY + Room->RoomHeight - 0.5f) * ModuleSize, SwitchHeight });

                Switches.Add(Cast<ALightButton>(GetWorld()->SpawnActor(SwitchType, &Transform, SpawnParameters)));

                break;
            }
        }

        for(int i = 0; i < Switches.Num(); i++)
        {
            for(int j = 0; j < Lights.Num(); j++)
            {
                Switches[i]->AddNeon(Lights[j]);
            }
        }
    }
}

void AUnrealProjectGameMode::GenerateItems()
{
    for(int i = 0; i < 6; i++)
    {
        bool bItemPlaced = false;
        int SafeIncrement = 0;
        while(!bItemPlaced)
        {
            UE_LOG(LogTemp, Warning, TEXT("item"));
            const int RoomIndex = FMath::RandRange(1, Rooms.Num() - 1);

            if(Rooms[RoomIndex]->ItemCount < MaxItemPerRoom || SafeIncrement > 10)
            {
                if(!bItemPlaced)
                {
                    const int ItemX = Rooms[RoomIndex]->CurrentX + FMath::RandRange(0, Rooms[RoomIndex]->RoomWidth - 1);
                    const int ItemY = Rooms[RoomIndex]->CurrentY + FMath::RandRange(0, Rooms[RoomIndex]->RoomHeight - 1);

                    if(BonusesGrid[XYToIndex(ItemX, ItemY)] == 0)
                    {
                        PlaceItem(ItemX, ItemY, i);
                        bItemPlaced = true;
                        Rooms[RoomIndex]->ItemCount++;
                    }
                }
            }
            SafeIncrement++;
        }
    }
}

void AUnrealProjectGameMode::PlaceItem(const int x, const int y, const int Type) const
{
    BonusesGrid[XYToIndex(x, y)] = Type;

    FActorSpawnParameters SpawnParameters;
    SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    const FTransform Transform(FRotator::ZeroRotator, FVector{ MazeX + x * ModuleSize, MazeY + y * ModuleSize, 50 });

    GetWorld()->SpawnActor(GetItemType(Type + 1), &Transform, SpawnParameters);
}

TSubclassOf<AItem> AUnrealProjectGameMode::GetItemType(int index) const
{
    switch (index)
    {
    case 1:
        return Item1Type;
    case 2:
        return Item2Type;
    case 3:
        return Item3Type;
    case 4:
        return Item4Type;
    case 5:
        return Item5Type;
    case 6:
        return Item6Type;
    default:
        return nullptr;
    }
}

void AUnrealProjectGameMode::GenerateObjects()
{
    const FVector Spawn = GetSpawnLocation();

    ObjectsGrid[XYToIndex(Spawn.X, Spawn.Y)] = EMPTY;

    for(int i = 0; i < Width; i++)
    {
        for(int j = 0; j < Height; j++)
        {
            switch(ObjectsGrid[XYToIndex(i, j)])
            {
            case ROOM_MIDDLE:
                GenerateRoomMiddleObject(i, j);
                break;
            case ROOM_WALL:
                GenerateRoomWallObject(i, j);
                break;
            case DEAD_END:
                GenerateDeadEndObject(i, j);
                    break;
            default:
                break;
            }
        }
    }
}

void AUnrealProjectGameMode::GenerateDeadEndObject(int x, int y)
{
    FRotator ObjectRotation = FRotator::ZeroRotator;
    FVector ObjectLocation = { MazeX + x * ModuleSize, MazeY + y * ModuleSize, 50 };

    if (Grid[XYToIndex(x, y - 1)] == GROUND)
    {
        ObjectRotation.Yaw = 0;
        ObjectLocation.Y += ModuleSize / 2;
    }
    else if (Grid[XYToIndex(x, y + 1)] == GROUND)
    {
        ObjectRotation.Yaw = 180;
        ObjectLocation.Y -= ModuleSize / 2;
    }
    else if (Grid[XYToIndex(x - 1, y)] == GROUND)
    {
        ObjectRotation.Yaw = 90;
        ObjectLocation.X += ModuleSize / 2;
    }
    else
    {
        ObjectRotation.Yaw = -90;
        ObjectLocation.X -= ModuleSize / 2;
    }

    PlaceBigObject(FTransform(ObjectRotation, ObjectLocation));
}

void AUnrealProjectGameMode::GenerateRoomMiddleObject(int x, int y)
{
    const auto Size = FMath::RandRange(0, 9);
    if(Size < 3)
    {
        FRotator ObjectRotation = FRotator::ZeroRotator;
        ObjectRotation.Yaw = FMath::RandRange(0, 379);
        const FTransform ObjectTransform = { ObjectRotation, { MazeX + x * ModuleSize, MazeY + y * ModuleSize, 50 } };

        if (Size == 0)
        {
            PlaceBigObject(ObjectTransform);
        }
        else
        {
            PlaceSmallObject(ObjectTransform);
        }
    }
}

void AUnrealProjectGameMode::GenerateRoomWallObject(int x, int y)
{
    const auto Size = FMath::RandRange(0, 9);
    if (FMath::RandRange(0, 9) == 0)
    {
        FRotator    ObjectRotation = FRotator::ZeroRotator;
        FVector     ObjectLocation = { MazeX + x * ModuleSize, MazeY + y * ModuleSize, 50 };

        if (Grid[XYToIndex(x, y - 1)] == WALL)
        {
            ObjectRotation.Yaw = -90;
            ObjectLocation.Y -= ModuleSize/2;
        }
        else if (Grid[XYToIndex(x, y + 1)] == WALL)
        {
            ObjectRotation.Yaw = 90;
            ObjectLocation.Y += ModuleSize/2;
        }
        else if (Grid[XYToIndex(x - 1, y)] == WALL)
        {
            ObjectRotation.Yaw = 0;
            ObjectLocation.X -= ModuleSize/2;
        }
        else
        {
            ObjectRotation.Yaw = 180;
            ObjectLocation.X += ModuleSize/2;
        }

        const FTransform ObjectTransform = { ObjectRotation, ObjectLocation };
        PlaceBigObject(ObjectTransform);
    }
}

void AUnrealProjectGameMode::PlaceBigObject(FTransform Transform)
{
    if(ObjectsComposition.Num() > 0)
    {
        const auto Type = FMath::RandRange(0, ObjectsComposition.Num() - 1);

        FActorSpawnParameters SpawnParameters;
        SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        GetWorld()->SpawnActor(ObjectsComposition[Type], &Transform, SpawnParameters);
    }
}

void AUnrealProjectGameMode::PlaceSmallObject(FTransform Transform)
{
    if (SimpleObjects.Num() > 0)
    {
        const auto Type = FMath::RandRange(0, SimpleObjects.Num() - 1);

        FActorSpawnParameters SpawnParameters;
        SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        GetWorld()->SpawnActor(SimpleObjects[Type], &Transform, SpawnParameters);
    }
}

void AUnrealProjectGameMode::SpawnEnemy() const
{
    bool bSpawned = false;

    while(!bSpawned)
    {
        const int EnemyX = FMath::RandRange(0, Width - 1);
        const int EnemyY = FMath::RandRange(0, Height - 1);


        if(Grid[XYToIndex(EnemyX, EnemyY)] == GROUND && FVector2D::Distance({ (float)EnemyX, (float)EnemyY }, { GetSpawnLocation().X, GetSpawnLocation().Y }) > MinEnemySpawnDistanceToPlayer)
        {
            FActorSpawnParameters SpawnParameters;
            SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

            const FTransform Transform(FRotator::ZeroRotator, FVector{ MazeX + EnemyX * ModuleSize, MazeY + EnemyY * ModuleSize, 50 });

            GetWorld()->SpawnActor(EnemyType, &Transform, SpawnParameters);

            bSpawned = true;
        }
    }
}

void AUnrealProjectGameMode::CheckWinConditions(AUnrealProjectCharacter* Player)
{
    if (Player)
    {
        for(int i = 0; i < 3; i++)
        {
            bool bFoundNeededItem = false;
            for (size_t j = 0; j < Player->Itens.Num(); j++)
            {
                if(Player->Itens[j] == NeededItems->ItensConditionWin[i])
                {
                    bFoundNeededItem = true;
                    break;
                }
            }
            if(!bFoundNeededItem)
            {
                return;
            }
        }

        OnWin.Broadcast();
    }
}

FVector AUnrealProjectGameMode::GetSpawnLocation() const
{
    return { float(Rooms[0]->CurrentX + Rooms[0]->RoomWidth / 2), float(Rooms[0]->CurrentY + Rooms[0]->RoomHeight / 2), 0 };
}
