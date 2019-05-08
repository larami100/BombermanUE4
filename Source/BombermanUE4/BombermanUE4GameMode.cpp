// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "BombermanUE4GameMode.h"
#include "BombermanUE4PlayerController.h"
#include "BombermanUE4Character.h"
#include "UObject/ConstructorHelpers.h"

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
}