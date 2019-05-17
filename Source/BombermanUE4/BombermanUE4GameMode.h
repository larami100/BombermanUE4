// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
//#include "Misc/Timespan.h"
#include "BombermanUE4GameMode.generated.h"

UCLASS(minimalapi)
class ABombermanUE4GameMode : public AGameModeBase
{
	GENERATED_BODY()

	FTimerHandle FuseTimerHandle;

	void DecreaseTimer();

public:
	ABombermanUE4GameMode();

	UFUNCTION(BlueprintCallable, Category = "UMG Game")
	void ChangeMenuWidget(TSubclassOf<UUserWidget> NewWidgetClass);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTimespan GameTimer;

	virtual void BeginPlay() override;

	// Removed for optimization
	// virtual void Tick(float DeltaTime) override;

	void OnGameOver();

	void UpdateTimeAndBombs(int32 Bombs);

protected:

	FTimerHandle GameTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UMG Game")
	TSubclassOf<UUserWidget> StartingWidgetClass;

	UPROPERTY()
	UUserWidget* CurrentWidget;

};



