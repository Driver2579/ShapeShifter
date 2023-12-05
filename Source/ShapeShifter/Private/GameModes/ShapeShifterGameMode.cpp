// Fill out your copyright notice in the Description page of Project Settings.

#include "GameModes/ShapeShifterGameMode.h"

#include "Actors/SaveGameManager.h"

void AShapeShifterGameMode::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (!SaveGameManagerClass)
	{
		UE_LOG(LogTemp, Error,
			TEXT("AShapeShifterGameMode::PostInitializeComponents: SaveGameManagerClass is invalid!"));

		return;
	}

	// We spawn SaveGameManager in PostInitializeComponents to make sure it spawned before all Actors called BeginPlay
	SaveGameManager = GetWorld()->SpawnActor<ASaveGameManager>(SaveGameManagerClass);

	if (!SaveGameManager.IsValid())
	{
		UE_LOG(LogTemp, Error,
			TEXT("AShapeShifterGameMode::PostInitializeComponents: Failed to spawn SaveGameManager!"));
	}
}

ASaveGameManager* AShapeShifterGameMode::GetSaveGameManager() const
{
	return SaveGameManager.Get();
}