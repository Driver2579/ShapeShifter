// Fill out your copyright notice in the Description page of Project Settings.

#include "Pawns/BallPawn.h"

#include "EnhancedInputComponent.h"

void ABallPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABallPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	if (!IsValid(EnhancedInputComponent))
	{
		UE_LOG(LogTemp, Error, TEXT("ABallPawn::SetupPlayerInputComponent: InputComponentClass must be overriden by EnhancedInputComponent!"));

		return;
	}
}