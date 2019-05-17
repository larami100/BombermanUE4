// Fill out your copyright notice in the Description page of Project Settings.


#include "Bomb.h"
#include "Components/StaticMeshComponent.h"
#include "CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "BombermanUE4Character.h"
#include "Particles/ParticleSystemComponent.h"
#include "WallsGenerator.h"

// Sets default values
ABomb::ABomb()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	TimeToExplode = 5.f;
	SetActorEnableCollision(false);
	
	BombMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BombMesh"));
	RootComponent = BombMeshComponent;
	
	// Set basic sphere form mesh asset to the wall (Asset file obtained from UE4 Starterpack content)

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMeshAsset(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere'"));
	if (SphereMeshAsset.Succeeded())
	{
		BombMeshComponent->SetStaticMesh(SphereMeshAsset.Object);
		SetActorEnableCollision(false);

		//Increase the bomb size
		BombMeshComponent->SetRelativeScale3D(FVector(1.5f, 1.5f, 1.5f));
	}
	
	
	static ConstructorHelpers::FObjectFinder<UMaterial> FoundMaterial(TEXT("Material'/Game/StarterContent/Materials/M_Bomb.M_Bomb'"));
	if (FoundMaterial.Succeeded())
	{
		BombMaterial = FoundMaterial.Object;
		UMaterialInstanceDynamic* DynamicMaterialInst = UMaterialInstanceDynamic::Create(BombMaterial, BombMeshComponent);
		BombMeshComponent->SetMaterial(0, DynamicMaterialInst);
	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem> PS(TEXT("ParticleSystem'/Game/StarterContent/Particles/P_Explosion.P_Explosion'"));
	ExplosionPSC = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("MyPSC"));
	ExplosionFX = PS.Object;
	ExplosionPSC->SetTemplate(ExplosionFX);
	ExplosionPSC->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ABomb::BeginPlay()
{
	Super::BeginPlay();

	PlaceBombInLogicBoard();

	//Give a lifetime of 10 seconds to the bomb 
	this->SetLifeSpan(10);
	
	//After the time indicated in TimeToExplode, call the Explode function 
	GetWorldTimerManager().SetTimer(FuseTimerHandle, this, &ABomb::Explode, TimeToExplode, false);
}

void ABomb::Explode()
{
	if (ExplosionFX)
	{
		PlayExplosionEffect();

		BombMeshComponent->SetVisibility(false);
		
		TArray<AActor*> ActorsMapArray;
		FVector2D BombLocation;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWallsGenerator::StaticClass(), ActorsMapArray);
		AWallsGenerator* ActorsMap = Cast<AWallsGenerator>(ActorsMapArray[0]);
		if(ActorsMap)
		{
			// Place the bomb in the logic board in order to check if it has adyacent objects that surrounds it in order to destroy them
			BombLocation = ActorsMap->Board.ChangeMapPositionToLogicBoardPosition(GetActorLocation());
			
			// Destroy pickups and destructable walls that surrounded the bomb 1 position
			ActorsMap->DestroyActor(BombLocation.X, BombLocation.Y, 1);
			
			// If the player has the More Bombs power-up
			if (bDoesHaveLongerExplosion)
			{
				//Destroy pickups an destructable walls that surrounded the bomb at second and third positions. 
				DestroyLongerExplosion();
			}
		}

		if (ABombermanUE4Character* Player = Cast<ABombermanUE4Character>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
		{
			// If the player was inside a bomb blast, destroy him
			DestroyPlayerInBlast();

			// Return the bomb counter to 1
			if (!Player->BombsCounter)
			{
				Player->BombsCounter = 1;
			}
		}
	}
}

void ABomb::DestroyPlayerInBlast()
{
	if (ABombermanUE4Character* Player = Cast<ABombermanUE4Character>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{
		float ExplosionLength = bDoesHaveLongerExplosion ? 600.f : 200.f;
		// Check if the player was inside a blast.
		if ((Player->GetActorLocation().X >= (GetActorLocation().X - ExplosionLength)) && (Player->GetActorLocation().X <= (GetActorLocation().X + ExplosionLength)) && (Player->GetActorLocation().Y >= (GetActorLocation().Y - 100.f)) && (Player->GetActorLocation().Y <= (GetActorLocation().Y + 100.f)))
		{
			if ((Player->GetActorLocation().Y >= (GetActorLocation().Y - ExplosionLength)) && (Player->GetActorLocation().Y <= (GetActorLocation().Y + ExplosionLength)) && (Player->GetActorLocation().X >= (GetActorLocation().X - 100.f)) && (Player->GetActorLocation().X <= (GetActorLocation().X + 100.f)))
			{
				((ABombermanUE4GameMode*)GetWorld()->GetAuthGameMode())->OnGameOver();
			}
		}
	}
}


void ABomb::DestroyLongerExplosion()
{
	TArray<AActor*> ActorsMapArray;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWallsGenerator::StaticClass(), ActorsMapArray);
	if (AWallsGenerator* ActorsMap = Cast<AWallsGenerator>(ActorsMapArray[0]))
	{
		FVector2D BoardLocation = ActorsMap->Board.ChangeMapPositionToLogicBoardPosition(GetActorLocation());

		// In longer blasts, check that the second blast position has a destructable object to be able to destroy the object at the third position
		if (ActorsMap->DestroyActorInBoard(BoardLocation.X - 1, BoardLocation.Y))
		{
			ActorsMap->DestroyActorInBoard(BoardLocation.X - 2, BoardLocation.Y);
		}
		if (ActorsMap->DestroyActorInBoard(BoardLocation.X + 1, BoardLocation.Y))
		{
			ActorsMap->DestroyActorInBoard(BoardLocation.X + 2, BoardLocation.Y);
		}
		if (ActorsMap->DestroyActorInBoard(BoardLocation.X, BoardLocation.Y - 1))
		{
			ActorsMap->DestroyActorInBoard(BoardLocation.X, BoardLocation.Y - 2);
		}
		if (ActorsMap->DestroyActorInBoard(BoardLocation.X, BoardLocation.Y + 1))
		{
			ActorsMap->DestroyActorInBoard(BoardLocation.X, BoardLocation.Y + 2);
		}
	}
}

void ABomb::PlayExplosionEffect()
{
	// Each square has 200 units per size, the size of a simple blast. Large blasts take 3 squares (600 units)
	float ExplosionLength = bDoesHaveLongerExplosion ? 600.f : 200.f;

	//Make the explosions displayed at X coordinate
	FVector ExplosionDirection = GetActorLocation(); 
	for (float ExplosionsX = GetActorLocation().X - ExplosionLength; ExplosionsX < (GetActorLocation().X + ExplosionLength); ExplosionsX += 20)
	{
		ExplosionDirection.X = ExplosionsX;
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionFX, ExplosionDirection, FRotator::ZeroRotator, true);
	}
	
	//Make the explosions displayed at Y coordinate
	ExplosionDirection = GetActorLocation();
	for (float ExplosionsY = GetActorLocation().Y - ExplosionLength; ExplosionsY < (GetActorLocation().Y + ExplosionLength); ExplosionsY += 20)
	{
		ExplosionDirection.Y = ExplosionsY;
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionFX, ExplosionDirection, FRotator::ZeroRotator, true);
	}
}

// Convert bomb map position to a position in the logic board. 
void ABomb::PlaceBombInLogicBoard()
{
	TArray<AActor*> ActorsMapArray;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWallsGenerator::StaticClass(), ActorsMapArray);
	if (AWallsGenerator* ActorsMap = Cast<AWallsGenerator>(ActorsMapArray[0]))
	{
		FVector2D BoardLocation =  ActorsMap->Board.ChangeMapPositionToLogicBoardPosition(GetActorLocation());
		ActorsMap->Board.Rows[BoardLocation.X].ActorsInGameBoard[BoardLocation.Y] = Cast<AActor>(this);
	}
}

