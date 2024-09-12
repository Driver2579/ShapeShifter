// Fill out your copyright notice in the Description page of Project Settings.

#include "GameInstances/ShapeShifterGameInstance.h"

void UShapeShifterGameInstance::OnStart()
{
	Super::OnStart();

	OnGameStartOrLevelChanged.Broadcast();
}

void UShapeShifterGameInstance::OnWorldChanged(UWorld* OldWorld, UWorld* NewWorld)
{
	Super::OnWorldChanged(OldWorld, NewWorld);

	if (IsValid(NewWorld))
	{
		OnGameStartOrLevelChanged.Broadcast();
	}
}