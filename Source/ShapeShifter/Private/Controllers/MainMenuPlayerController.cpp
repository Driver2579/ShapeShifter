// Fill out your copyright notice in the Description page of Project Settings.

#include "Controllers/MainMenuPlayerController.h"

AMainMenuPlayerController::AMainMenuPlayerController()
{
	bShowMouseCursor = true;
}

void AMainMenuPlayerController::BeginPlay()
{
	Super::BeginPlay();

	SetInputMode(FInputModeUIOnly());
}