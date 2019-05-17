// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "BombermanUE4GameMode.h"
#include "BombermanUE4PlayerController.h"
#include "BombermanUE4Character.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"
#include "BombermanUserWidget.h"

void ABombermanUE4GameMode::DecreaseTimer()
{
	//Substract one second to the timer 
	GameTimer -= FTimespan(0, 0, 1);
	if (ABombermanUE4Character* Player = Cast<ABombermanUE4Character>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{
		//Set game timer and bombs counter values in the interface 
		((UBombermanUserWidget*)CurrentWidget)->SetTimeAndBombs(GameTimer, Player->BombsCounter);

		// If the game timer is 0:00
		if (!GameTimer.GetMinutes() && !GameTimer.GetSeconds())
		{
			// End game
			OnGameOver();
		}
	}
}

ABombermanUE4GameMode::ABombermanUE4GameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = ABombermanUE4PlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDownCPP/Blueprints/TopDownCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	//Set the game timer to 5 minutes
	GameTimer = FTimespan(0, 5, 0);
}

void ABombermanUE4GameMode::ChangeMenuWidget(TSubclassOf<UUserWidget> NewWidgetClass)
{
	//If already exist a widget in the interface
	if (CurrentWidget != nullptr)
	{
		// Remove it from the screen
		CurrentWidget->RemoveFromViewport();
		CurrentWidget = nullptr;
	}
	if (NewWidgetClass != nullptr)
	{
		CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), NewWidgetClass);
		if (CurrentWidget != nullptr)
		{
			CurrentWidget->AddToViewport();
		}
	}
}

void ABombermanUE4GameMode::BeginPlay()
{
	Super::BeginPlay();
	ChangeMenuWidget(StartingWidgetClass);
	((UBombermanUserWidget*)CurrentWidget)->Load();

	// Call each second the DecreaseTimer function in order to decrease the game timer
	GetWorldTimerManager().SetTimer(FuseTimerHandle, this, &ABombermanUE4GameMode::DecreaseTimer, 1.f, true);
}

void ABombermanUE4GameMode::OnGameOver()
{
	// Pause the game
	UGameplayStatics::SetGamePaused(GetWorld(), true);

	// Display Game over message
	((UBombermanUserWidget*)CurrentWidget)->OnGameOver();
}

void ABombermanUE4GameMode::UpdateTimeAndBombs(int32 Bombs)
{
	// Update Game timer and bombs values displayed at the screen 
	((UBombermanUserWidget*)CurrentWidget)->SetTimeAndBombs(GameTimer, Bombs);
}


