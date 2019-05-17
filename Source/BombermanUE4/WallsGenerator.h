// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Wall.h"
#include "WallsGenerator.generated.h"

// USTRUCT to hold how many columns the map will have
USTRUCT(BlueprintType)
struct FBoardGridRow {

	GENERATED_USTRUCT_BODY()


	//UE4 variable to set the kind of wall that is going to be spawned in that column of the board 
	UPROPERTY()
	TArray<EWallTypesEnum> Columns;
	TArray<AActor*> ActorsInGameBoard;

	void AddNewColumn()
	{
		// Set that no wall is in this map position
		Columns.Add(EWallTypesEnum::WE_None);
	}

	void AddActorsInGameBoard(AActor* ActorSpawned)
	{
		ActorsInGameBoard.Add(ActorSpawned);
	}
};

// USTRUCT to hold how many rows the map will have
USTRUCT(BlueprintType)
struct FBoardGrid
{
	GENERATED_USTRUCT_BODY()

	//UE4 variable to set the kind of wall that is going to be spawned in that row of the board 
	UPROPERTY()
	TArray<FBoardGridRow> Rows;

	// Probability that a destructable wall could have a power up inside it
	float PercentageToSPawnPickup;

	// Total rows and columns that the map will have
	int32 TotalRows;
	int32 TotalColumns;
	float InitialPositionX;
	float InitialPositionY;

	FBoardGrid()
	{
		TotalRows = 13;
		TotalColumns = 19;
		InitialPositionX = 1045.f;
		InitialPositionY = -1858.f;
	}

	void AddNewRow()
	{
		Rows.Add(FBoardGridRow());
	}

	// Set initial state of the map, which is a clean map with any walls spawned.
	void AddUninitialized(const int32 RowCount, const int32 ColCount)
	{
		//Add Rows
		for (int32 Row = 0; Row < RowCount; Row++)
		{
			AddNewRow();
		}

		//Add Columns
		for (int32 Row = 0; Row < RowCount; Row++)
		{
			for (int32 Column = 0; Column < ColCount; Column++)
			{
				Rows[Row].AddNewColumn();
			}
		}
	}

	// Generate destructable walls or empty spaces randomly. Generate destructable walls in fixed positions. 
	void GenerateBoard(const int32 RowCount, const int32 ColCount)
	{

		//Temporary UE4 TArray to hold the positions of the destructable walls that the map will have.
		TArray<FVector2D> DestructableWalls;

		// Number of destructable walls that the map will have. I have to create this variable because DestructableWalls.Num() returns unestable values at running time :(
		int32 DestructablesCount = 0;

		for (int32 Row = 0; Row < RowCount; Row++)
		{
			for (int32 Column = 0; Column < ColCount; Column++)
			{
				// Set a undestructable wall in the logic board if both row and column are at odd position
				if (Row % 2 != 0 && Column % 2 != 0)
				{
					Rows[Row].Columns[Column] = EWallTypesEnum::WE_Undestructable;
				}
				//Left the map corners without walls in order to place the 4 players (one at each corner)
				else if (((Row == 0 || Row == RowCount - 1) && (Column == 0 || Column == 1 || Column == ColCount - 2 || Column == ColCount - 1)) ||
					((Row == 1 || Row == RowCount - 2) && (Column == 0 || Column == ColCount - 1)))
				{
					Rows[Row].Columns[Column] = EWallTypesEnum::WE_None;
				}
				// Set randomly an empty space or a destructable wall
				else
				{
					EWallTypesEnum WallType = static_cast<EWallTypesEnum>(FMath::RandRange(1, 2));
					Rows[Row].Columns[Column] = WallType;
					// If a destructable wall is selected, save its coordinates in a TArray
					if (WallType == EWallTypesEnum::WE_Destructable)
					{
						DestructableWalls.Add(FVector2D(Row, Column));
						DestructablesCount++;
					}
				}
			}
		}

		// int PossibleDestructables = DestructablesCount * .3;

		// DestructablesCount holds the total of destructable walls generated randomly. To determine how many will have a power up inside it, I multiply the total by the percentage allowed to generate a power up (30%)
		for (int32 PossibleDestructables = DestructablesCount * PercentageToSPawnPickup; PossibleDestructables > 0; --PossibleDestructables)
		{
			// Next I get randomly a position of all the destructable walls generated
			int32 RandomDestructableWall = FMath::RandRange(0, DestructablesCount);
			FVector2D DestructableWallPosition = DestructableWalls[RandomDestructableWall];

			// Then I remove that position in the temporary TArray so the next cycle that position will not be considered to generate another random position
			DestructableWalls.RemoveAt(RandomDestructableWall);
			--DestructablesCount;

			// Set a ramdom power up in the ramdom position get from temporary TArray
			EWallTypesEnum WallType = static_cast<EWallTypesEnum>(FMath::RandRange(3, 4));
			Rows[DestructableWallPosition.X].Columns[DestructableWallPosition.Y] = WallType;
		}
	}

	FVector2D ChangeMapPositionToLogicBoardPosition(const FVector ActorLocation)
	{
		UE_LOG(LogTemp, Warning, TEXT("Actr Lcatin in map: %.2f, %.2f"), ActorLocation.X, ActorLocation.Y)
		int32 Row = -1, Column = -1;
		for (float X = InitialPositionX; X > ActorLocation.X; X -= 200, ++Row);
		for (float Y = InitialPositionY; Y < ActorLocation.Y; Y += 200, ++Column);

		Row = Row < 0 ? 0 : Row;
		Column = Column < 0 ? 0 : Column;
		Row = Row >= TotalRows ? (TotalRows-1) : Row;
		Column = Column >= TotalColumns ? (TotalColumns-1) : Column;

		UE_LOG(LogTemp, Warning, TEXT("Actr Lcatin in bard: %d, %d"), Row, Column)

		return FVector2D(Row, Column);
	}

};

UCLASS()
class BOMBERMANUE4_API AWallsGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWallsGenerator();

	// Logic Board that holds the walls that are going to be spawned in the map
	UPROPERTY()
	FBoardGrid Board;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PercentageToSPawnPickup;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TotalRows;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TotalColumns;

	void DestroyActor(int32 Row, int32 Column, int32 Position);

	bool DestroyActorInBoard(const int32 Row, const int32 Column);

	void LogBoard();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame. Turned off to improve performance
	// virtual void Tick(float DeltaTime) override;

};
