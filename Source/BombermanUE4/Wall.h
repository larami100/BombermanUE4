// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BombermanUE4GameMode.h"
#include "Wall.generated.h"

// Enumeration to hold the different types of walls than can be spawned. WE_None refers that no wall is going to be spawned.
UENUM(BlueprintType)
enum class EWallTypesEnum : uint8
{
	WE_Undestructable  	UMETA(DisplayName = "Indestructable"),
	WE_None 			UMETA(DisplayName = "None"),
	WE_Destructable  	UMETA(DisplayName = "Destructable"),
	WE_LongerBombBlasts	UMETA(DisplayName = "LongerBombBlasts"),
	WE_MoreBombs		UMETA(DisplayName = "MoreBombs"),
	WE_FasterRunSpeed	UMETA(DisplayName = "FasterRunSpeed")
};

// Forward declaration in order to avoid the inclusion of the header file
class UMaterial;

UCLASS()
class BOMBERMANUE4_API AWall : public AActor
{
	GENERATED_BODY()
	
public:	

	// Editable property in blueprint to hold the kind of the wall (if it is undestructable, destructable or if it will have a power up inside it) 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EWallTypesEnum WallTypesEnum;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* WallMeshComponent;

	// UE4 Variables to hold the two possible materials a wall could have in order to differenciate them.
	UPROPERTY()
	UMaterial* StoredMaterialDestructable;

	UPROPERTY()
	UMaterial* StoredMaterialUndestructable;

	// Sets default values for this actor's properties
	AWall();

	void SetWallType(EWallTypesEnum WallTypesEnum);

	// Set wall material based on the wall type
	void SetMaterial(EWallTypesEnum WallTypesEnumCreated);

	// Called every frame. Turned off to improve performance
	// virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	// virtual void BeginPlay() override;

	// Called when the game ends or when destroy
	virtual void Destroyed() override;


};
