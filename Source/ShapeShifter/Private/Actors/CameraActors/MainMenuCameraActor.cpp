// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/CameraActors/MainMenuCameraActor.h"

#include "Kismet/GameplayStatics.h"

void AMainMenuCameraActor::BeginPlay()
{
	Super::BeginPlay();

	// Get Controller to set view target
	
	APlayerController* Controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	
	if (!IsValid(Controller))
	{
		UE_LOG(LogTemp, Error, TEXT("AMainMenuCameraActor::BeginPlay: Controller is invalid!"));

		return;
	}

	// Pull over view target
	Controller->SetViewTargetWithBlend(this);
}
