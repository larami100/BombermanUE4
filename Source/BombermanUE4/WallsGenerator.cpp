// Fill out your copyright notice in the Description page of Project Settings.


#include "WallsGenerator.h"
#include "Wall.h"
#include "Kismet/GameplayStatics.h"
#include "Engine.h"
#include "PowerUpPickUp.h"
#include "BombermanUE4Character.h"

// Sets default values
AWallsGenerator::AWallsGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	// PrimaryActorTick.bCanEverTick = true;

	// Set the number of walls that the map will have at each row and each column
	TotalRows = 13;
	TotalColumns = 19;

	// Set the probability that a destructable wall could have a power up inside it
	PercentageToSPawnPickup = 0.3f;

	// Set initial state of the map, which is a clean map with any walls spawned.
	Board.AddUninitialized(TotalRows, TotalColumns);

	// Generate dynamically the walls 
	Board.GenerateBoard(TotalRows, TotalColumns);
}

// Called when the game starts or when spawned
// The content of this method is here because in BeginPlay method is guaranteed that the world is created and is safe to spawn actors.
void AWallsGenerator::BeginPlay()
{
	Super::BeginPlay();

	//UE4 variables required to spawn the actor in the map
	FRotator Rotation(0.f, 0.f, 0.f);
	FVector Scale(1.f, 1.f, 1.f);

	// Position of the first wall that is going to be spawned in the map, which is the one localized at the top-left corner.
	float X = 1045.f;
	float Z = 367.f;

	// Check each position of the logic board to spawn the walls
	for (int32 Row = 0; Row < TotalRows; Row++)
	{
		float Y = -1800.f;
		for (int32 Column = 0; Column < TotalColumns; Column++)
		{
			// Set the location where the a new wall is going to be spawned. Z value is fixed since all the walls are spawned at the same Z coordinate.
			FVector Location(X, Y, Z);

			//Location where a wall is going to be spawned. (Variable required in SpawnActorDeferred function call).
			FTransform Transform(Rotation, Location, Scale);

			// If the logic board indicate that there is no wall at this position, skip this code
			if (Board.Rows[Row].Columns[Column] != EWallTypesEnum::WE_None)
			{

				AWall* WallSpawned = GetWorld()->SpawnActorDeferred<AWall>(AWall::StaticClass(), Transform, this, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
				if (WallSpawned)
				{	
					// Modify wall type and material before spawning it on the map
					WallSpawned->SetWallType(Board.Rows[Row].Columns[Column]);
					WallSpawned->SetMaterial(Board.Rows[Row].Columns[Column]);
					UGameplayStatics::FinishSpawningActor(WallSpawned, Transform);
					Board.Rows[Row].AddActorsInGameBoard(WallSpawned);
				}
			}
			else
			{
				Board.Rows[Row].AddActorsInGameBoard(nullptr);
			}
			// Change the position where the next wall is going to be spawned at Y coordinate. Each wall is 200 units size, so the next wall must be spawned 200 units after the one previously spawned
			Y += 200.f;
		}
		// The same case of the previous comment but with X coordinate :)
		X -= 200.f;
	}
}

// Destroy a wall or power-up in the four blast explosion positions that a bomb has.
void AWallsGenerator::DestroyActor(int32 Row, int32 Column, int32 Position)
{
	if (Row - Position >= 0)
	{
		DestroyActorInBoard(Row - Position, Column);
	}
	if (Row + Position <= (TotalRows - 1))
	{
		DestroyActorInBoard(Row + Position, Column);
	}
	if (Column - Position >= 0)
	{
		DestroyActorInBoard(Row, Column - Position);
	}
	if (Column + Position <= (TotalColumns - 1))
	{
		DestroyActorInBoard(Row, Column + Position);
	}
}

void AWallsGenerator::LogBoard()
{
	for (int32 Row = 0; Row < TotalRows; Row++)
	{
		for (int32 Column = 0; Column < TotalColumns; Column++)
		{
			const UEnum* WallTypesEnumX = FindObject<UEnum>(ANY_PACKAGE, TEXT("EWallTypesEnum"));
			
			UE_LOG(LogTemp, Warning, TEXT("Board in %d, %d is %d")
				, Row
				, Column
				, static_cast<int>(Board.Rows[Row].Columns[Column]));
		}
	}
}


// Check in the logic board if the position indicated by Row and Column has a destructable object in order to destroy it in the board and world map 
bool AWallsGenerator::DestroyActorInBoard(const int32 Row, const int32 Column)
{
	// LogBoard();

    //Condition to check valid board positions in longer bomb blasts
	if(Row >=0 && Row < TotalRows && Column >=0 && Column < TotalColumns)
	{
		if (Board.Rows[Row].ActorsInGameBoard[Column])
		{
			AWall* WallToDestroy = Cast<AWall>(Board.Rows[Row].ActorsInGameBoard[Column]);
			if (WallToDestroy && WallToDestroy->IsA(AWall::StaticClass()))
			{
				if ((WallToDestroy->WallTypesEnum != EWallTypesEnum::WE_None) && (WallToDestroy->WallTypesEnum != EWallTypesEnum::WE_Undestructable))
				{
					WallToDestroy->Destroy();
					Board.Rows[Row].Columns[Column] = EWallTypesEnum::WE_None;
					return true;
				}
			}
			else
			{
				APowerUpPickUp* PickUpToDestroy = Cast<APowerUpPickUp>(Board.Rows[Row].ActorsInGameBoard[Column]);
				if (PickUpToDestroy && PickUpToDestroy->IsA(APowerUpPickUp::StaticClass()))
				{
					Board.Rows[Row].Columns[Column] = EWallTypesEnum::WE_None;
					PickUpToDestroy->Destroy();
					return true;
				}
			}
		}
	}
	return false;
}