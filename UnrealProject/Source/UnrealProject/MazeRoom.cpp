// Fill out your copyright notice in the Description page of Project Settings.


#include "MazeRoom.h"
#include "UnrealProjectGameMode.h"

std::random_device UMazeRoom::Rd;
std::mt19937 UMazeRoom::Gen(Rd());

UMazeRoom::UMazeRoom()
{

}

UMazeRoom::~UMazeRoom()
{
    delete[] RoomWalls;
}

void UMazeRoom::SetWalls(AUnrealProjectGameMode* Mode, int* Walls, const int Height, const int Width, int Index)
{
    delete[] RoomWalls;

    RoomWalls = new int[Height * Width];

    for (int i = 0; i < Height * Width; i++)
    {
        RoomWalls[i] = Walls[i];
    }

    RoomHeight = Height;
    RoomWidth = Width;

    GameMode = Mode;

    RoomIndex = Index + 10;
}

bool UMazeRoom::GenerateRoom()
{
    const std::uniform_int_distribution<int> DisHeight(1, GameMode->Height - RoomHeight - 2);
    const std::uniform_int_distribution<int> DisWidth(1, GameMode->Width - RoomWidth - 2);

    for(int i = 0; i < 50; i++)
    {
        const int x = DisWidth(Gen);
        const int y = DisHeight(Gen);

        bool bInvalid = false;

        for(const auto& Room : GameMode->Rooms)
        {
            if(Room->CurrentX != -1 && x < Room->CurrentX + Room->RoomWidth && x + RoomWidth > Room->CurrentX && y < Room->CurrentY + Room->RoomHeight && y + RoomHeight > Room->CurrentY)
            {
                bInvalid = true;
                break;
            }
        }

        if(!bInvalid)
        {
            CreateRoom(x, y);
            return true;
        }
    }

    return false;
}

void UMazeRoom::CreateRoom(const int x, const int y)
{

    for(int i = 0; i < RoomWidth; i++)
    {
        for(int j = 0; j < RoomHeight; j++)
        {
            //if(RoomWalls[GameMode->XYToIndex(i, j)] == ROOM_GROUND)
            if(true)
            {
                GameMode->Grid[GameMode->XYToIndex(i + x, j + y)] = GROUND;
                GameMode->RoomsGrid[GameMode->XYToIndex(i + x, j + y)] = RoomIndex;
                GameMode->ObjectsGrid[GameMode->XYToIndex(i + x, j + y)] = 2;
            }
        }
    }

    CurrentX = x;
    CurrentY = y;
}
