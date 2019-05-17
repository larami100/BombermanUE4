// Fill out your copyright notice in the Description page of Project Settings.


#include "BombermanUserWidget.h"

void UBombermanUserWidget::Load()
{
	// some location in code that is after the widgettree has been constructed

	const FName GameOverTextBlockName = FName(TEXT("TimeBombsText"));

	if (GameOverText == nullptr)
	{
		GameOverText = (UTextBlock*)(WidgetTree->FindWidget(GameOverTextBlockName));
	}
}

void UBombermanUserWidget::SetTimeAndBombs(FTimespan Time, int32 Bombs)
{
	if (GameOverText != nullptr)
	{
		FString Seconds = FString::FromInt(Time.GetSeconds());
		if (Time.GetSeconds() < 10)
		{
			Seconds = FString(TEXT("0")) + Seconds;
		}
		GameOverText->SetText(FText::FromString(FString(TEXT("Time: ")) + FString::FromInt(Time.GetMinutes()) + FString(TEXT(":")) + Seconds + FString(TEXT("\nBombs: ")) + FString::FromInt(Bombs)));
	}
}

void UBombermanUserWidget::OnGameOver()
{
	if (GameOverText != nullptr)
	{
		GameOverText->SetText(FText::FromString(FString(TEXT("Game over: Press R to Restart."))));
	}
}

