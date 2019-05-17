// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UMG.h"
#include "UMGStyle.h"
#include "Slate/SObjectWidget.h"
#include "IUMGModule.h"
#include "BombermanUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class BOMBERMANUE4_API UBombermanUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY()
	UTextBlock* GameOverText;

	/** Called when the game starts. */
	void Load();
	void OnGameOver();
	void SetTimeAndBombs(FTimespan Time, int32 Bombs);

	
};
