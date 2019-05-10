// Fill out your copyright notice in the Description page of Project Settings.


#include "Wall.h"
#include "Runtime/Engine/Classes/Components/StaticMeshComponent.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Runtime/Engine/Classes/Materials/MaterialInstanceDynamic.h"

// Sets default values
AWall::AWall()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StoredMaterialDestructable = nullptr;
	StoredMaterialUndestructable = nullptr;

	WallMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WallMesh"));
	WallMeshComponent->SetupAttachment(RootComponent);

	// Set basic cube form mesh asset to the wall
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMeshAsset(TEXT("StaticMesh'/Game/Geometry/Meshes/1M_Cube_Chamfer.1M_Cube_Chamfer'"));
	if (CubeMeshAsset.Succeeded())	
	{
		PrimaryActorTick.bCanEverTick = true;
		WallMeshComponent->SetStaticMesh(CubeMeshAsset.Object);
		SetActorEnableCollision(true);

		//Double the cube asset original size
		WallMeshComponent->SetRelativeScale3D(FVector(2.f, 2.f, 4.f));
	}

	// Store destructable and undestructable wall materials in memory here at the Wall constructor because "static ConstructorHelpers" can't be called later if the wall is created dynamicaly with SpawnActorDeferred function
	static ConstructorHelpers::FObjectFinder<UMaterial> FoundMaterial1(TEXT("Material'/Game/StarterContent/Materials/M_Wood_Floor_Walnut_Polished.M_Wood_Floor_Walnut_Polished'"));
	if (FoundMaterial1.Succeeded())
	{
		StoredMaterialDestructable = FoundMaterial1.Object;
	}
	static ConstructorHelpers::FObjectFinder<UMaterial> FoundMaterial2(TEXT("Material'/Game/StarterContent/Materials/M_Brick_Clay_Old.M_Brick_Clay_Old'"));
	if (FoundMaterial2.Succeeded())
	{
		StoredMaterialUndestructable = FoundMaterial2.Object;
	}
}


void AWall::SetMaterial(EWallTypesEnum WallTypesEnum)
{
	//Based on the type of wall, the material is dynamically assigned
	switch (WallTypesEnum)
	{
		case EWallTypesEnum::WE_Destructable:
		case EWallTypesEnum::WE_FasterRunSpeed:
		case EWallTypesEnum::WE_LongerBombBlasts:
		case EWallTypesEnum::WE_MoreBombs:
		{
			if (StoredMaterialDestructable)
			{
				UMaterialInstanceDynamic* DynamicMaterialInst = UMaterialInstanceDynamic::Create(StoredMaterialDestructable, WallMeshComponent);
				WallMeshComponent->SetMaterial(0, DynamicMaterialInst);
			}
			break;
		}

		case EWallTypesEnum::WE_Undestructable:
		{
			if (StoredMaterialUndestructable)
			{
				UMaterialInstanceDynamic* DynamicMaterialInst = UMaterialInstanceDynamic::Create(StoredMaterialUndestructable, WallMeshComponent);
				WallMeshComponent->SetMaterial(0, DynamicMaterialInst);
			}
			break;
		}
	}

	// Release the memory assigned to store materials reference because they were set to the mesh at the previous lines.
	StoredMaterialDestructable = nullptr;
	StoredMaterialUndestructable = nullptr;
}

void AWall::SetWallType(EWallTypesEnum WallType)
{
	WallTypesEnum = WallType;
}

// Called when the game starts or when spawned
void AWall::BeginPlay()
{	
}

// Called every frame
void AWall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

