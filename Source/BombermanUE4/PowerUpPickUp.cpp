// Fill out your copyright notice in the Description page of Project Settings.


#include "PowerUpPickUp.h"
#include "BombermanUE4Character.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Kismet/GameplayStatics.h"
#include "WallsGenerator.h"

// Sets default values
APowerUpPickUp::APowerUpPickUp()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	// PrimaryActorTick.bCanEverTick = true;

	MaxBombsNumber = 3;
	MaxLongerExplosions = 3;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collision Component"));
	CollisionComponent->InitSphereRadius(200.0f);
	CollisionComponent->SetEnableGravity(true);
	CollisionComponent->SetSimulatePhysics(false);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	RootComponent = CollisionComponent;

	PickUpMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BombMesh"));
	PickUpMeshComponent->SetupAttachment(RootComponent);
	
	// Set basic sphere form mesh asset to the PickUp (Asset file obtained from UE4 Starterpack content)
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMeshAsset(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere'"));
	if (SphereMeshAsset.Succeeded())
	{
		PickUpMeshComponent->SetStaticMesh(SphereMeshAsset.Object);

		//Increase the power-up size
		PickUpMeshComponent->SetRelativeScale3D(FVector(1.5f, 1.5f, 1.5f));
	}

	//Choose randomly which power-up is going to be created
	PickUpType = static_cast<EPickUpTypesEnum>(FMath::RandRange(0, 1));

	//Assign the pickup material depending of the type of the power-up
	//All the code is included here because the action of changing the material is a task that only the constructor could make
	switch (PickUpType)
	{
		case EPickUpTypesEnum::PE_LongerBombBlasts:
		{
			static ConstructorHelpers::FObjectFinder<UMaterial> FoundMaterial1(TEXT("Material'/Game/StarterContent/Materials/M_Longer_Blasts_Powerup.M_Longer_Blasts_Powerup'"));
			if (FoundMaterial1.Succeeded())
			{
				PickUpMaterial = FoundMaterial1.Object;
			}
			break;
		}
		case EPickUpTypesEnum::PE_MoreBombs:
		{

			static ConstructorHelpers::FObjectFinder<UMaterial> FoundMaterial2(TEXT("Material'/Game/StarterContent/Materials/M_More_Bombs_Powerup.M_More_Bombs_Powerup'"));
			if (FoundMaterial2.Succeeded())
			{
				PickUpMaterial = FoundMaterial2.Object;
			}
			break;
		}
	}

	if (PickUpMaterial)
	{
		UMaterialInstanceDynamic* DynamicMaterialInst = UMaterialInstanceDynamic::Create(PickUpMaterial, PickUpMeshComponent);
		PickUpMeshComponent->SetMaterial(0, DynamicMaterialInst);
	}
}

// Called when the game starts or when spawned
void APowerUpPickUp::BeginPlay()
{
	Super::BeginPlay();
	PlacePickUpInLogicBoard();
	
}

void APowerUpPickUp::NotifyActorBeginOverlap(AActor * OtherActor)
{
	// If the player touches the pickup, apply the power-up and destroy it.
	if (ABombermanUE4Character* Player = Cast<ABombermanUE4Character>(OtherActor))
	{
		ApplyPickUp();
		Destroy();
	}
}

void APowerUpPickUp::ApplyPickUp()
{
	if (ABombermanUE4Character* Player = Cast<ABombermanUE4Character>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{
		switch (PickUpType)
		{
			/* TODO: Implement Faster Run Speed
			case EPickUpTypesEnum::PE_FasterRunSpeed:
			{
				
				break;
			}
			*/
			case EPickUpTypesEnum::PE_LongerBombBlasts:
			{
				// Assign 3 larger explosions to the player. 
				Player->LongerExplosions = MaxLongerExplosions;
				Player->BombsCounter = 1;
				((ABombermanUE4GameMode*)GetWorld()->GetAuthGameMode())->UpdateTimeAndBombs(Player->BombsCounter);
				break;
			}
			case EPickUpTypesEnum::PE_MoreBombs:
			{
				//Give 3 bombs to the player that could be spawned simultaneously
				Player->LongerExplosions = 0;
				Player->BombsCounter = MaxBombsNumber;
				((ABombermanUE4GameMode*)GetWorld()->GetAuthGameMode())->UpdateTimeAndBombs(Player->BombsCounter);
				break;
			}
		}
	}
}

void APowerUpPickUp::PlacePickUpInLogicBoard()
{
	int32 PlayerRow = -1, PlayerColumn = -1;
	for (float X = 1045.f; X > GetActorLocation().X; X -= 200, ++PlayerRow);
	for (float Y = -1858.f; Y < GetActorLocation().Y; Y += 200, ++PlayerColumn);
	PlayerRow = PlayerRow < 0 ? 0 : PlayerRow;
	PlayerColumn = PlayerColumn < 0 ? 0 : PlayerColumn;
	PlayerRow = PlayerRow > 12 ? 12 : PlayerRow;
	PlayerColumn = PlayerColumn > 18 ? 18 : PlayerColumn;
	TArray<AActor*> ActorsMapArray;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWallsGenerator::StaticClass(), ActorsMapArray);
	if (AWallsGenerator* ActorsMap = Cast<AWallsGenerator>(ActorsMapArray[0]))
	{
		ActorsMap->Board.Rows[PlayerRow].ActorsInGameBoard[PlayerColumn] = Cast<AActor>(this);
	}
}
