// Fill out your copyright notice in the Description page of Project Settings.

#include "GameModes/ShapeShifterGameMode.h"

#include "Actors/SaveGameManager.h"

void AShapeShifterGameMode::BeginPlay()
{
	Super::BeginPlay();

	SaveGameManager = GetWorld()->SpawnActor<ASaveGameManager>();
}

ASaveGameManager* AShapeShifterGameMode::GetSaveGameManager() const
{
	return SaveGameManager.Get();
}
