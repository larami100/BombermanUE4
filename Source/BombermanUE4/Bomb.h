// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bomb.generated.h"

class UMaterial;
class ABombermanUE4Character;

UCLASS()
class BOMBERMANUE4_API ABomb : public AActor
{
	GENERATED_BODY()

	void Explode();

	void PlayExplosionEffect();

	void PlaceBombInLogicBoard();

	void DestroyLongerExplosion();

	void DestroyPlayerInBlast();

	
public:	
	// Sets default values for this actor's properties
	ABomb();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Removed for optimization
	// virtual void Tick(float DeltaTime) override;
	
	UStaticMeshComponent*	  BombMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterial* BombMaterial;

	FTimerHandle FuseTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TimeToExplode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bDoesHaveLongerExplosion;

	/** The particle system of the explosion */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UParticleSystem* ExplosionFX;

	UPROPERTY(VisibleAnywhere)
	class UParticleSystemComponent* ExplosionPSC;


};
