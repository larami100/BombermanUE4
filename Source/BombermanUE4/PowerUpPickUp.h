// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PowerUpPickUp.generated.h"

// Enumeration to hold the different types of walls than can be spawned. WE_None refers that no wall is going to be spawned.
UENUM(BlueprintType)
enum class EPickUpTypesEnum : uint8
{
	PE_LongerBombBlasts	UMETA(DisplayName = "LongerBombBlasts"),
	PE_MoreBombs		UMETA(DisplayName = "MoreBombs"),
	// PE_FasterRunSpeed	UMETA(DisplayName = "FasterRunSpeed")
};

class USphereComponent;
class UMaterial;

UCLASS()
class BOMBERMANUE4_API APowerUpPickUp : public AActor
{
	GENERATED_BODY()

	void ApplyPickUp();

	void PlacePickUpInLogicBoard();

	
public:	
	// Sets default values for this actor's properties
	APowerUpPickUp();
	
	USphereComponent* CollisionComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* PickUpMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterial* PickUpMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EPickUpTypesEnum PickUpType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxBombsNumber;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxLongerExplosions;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame. Turned off to improve performance
	// virtual void Tick(float DeltaTime) override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;


};
