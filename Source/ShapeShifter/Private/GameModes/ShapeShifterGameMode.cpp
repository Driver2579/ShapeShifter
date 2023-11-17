// Fill out your copyright notice in the Description page of Project Settings.

#include "GameModes/ShapeShifterGameMode.h"

#include "Actors/SaveGameManager.h"

void AShapeShifterGameMode::StartPlay()
{
	Super::StartPlay();

	// We spawn SaveGameManager in StartPlay to make sure it spawned before all Actors called BeginPlay
	SaveGameManager = GetWorld()->SpawnActor<ASaveGameManager>();
}

ASaveGameManager* AShapeShifterGameMode::GetSaveGameManager() const
{
	return SaveGameManager.Get();
}