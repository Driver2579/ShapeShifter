// Fill out your copyright notice in the Description page of Project Settings.

#include "Controllers/ShapeShifterPlayerController.h"

#include "UI/HUDs/ShapeShifterHUD.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/GameModeBase.h"

void AShapeShifterPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Get LocalPlayer to get Subsystem
	const ULocalPlayer* LocalPlayer = GetLocalPlayer();

	if (!IsValid(LocalPlayer))
	{
		UE_LOG(LogTemp, Error, TEXT("AShapeShifterPlayerController::SetupInputComponent: LocalPlayer is invalid!"));

		return;
	}

	// Get LocalPlayerSubsystem
	UEnhancedInputLocalPlayerSubsystem* LocalPlayerSubsystem = LocalPlayer->GetSubsystem<
		UEnhancedInputLocalPlayerSubsystem>();

	if (!IsValid(LocalPlayerSubsystem))
	{
		UE_LOG(LogTemp, Error, TEXT("AShapeShifterPlayerController::SetupInputComponent: Subsystem is invalid!"));

		return;
	}

	// Add DefaultMappingContext to LocalPlayerSubsystem if it wasn't added before
	if (!LocalPlayerSubsystem->HasMappingContext(ShapeShifterMappingContext))
	{
		LocalPlayerSubsystem->AddMappingContext(ShapeShifterMappingContext, 0);
	}
}

void AShapeShifterPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);

	if (!IsValid(EnhancedInputComponent))
	{
		return;
	}

	if (IsValid(PauseAction))
	{
		EnhancedInputComponent->BindAction(PauseAction, ETriggerEvent::Triggered, this, 
			&AShapeShifterPlayerController::OnPause);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("AShapeShifterPlayerController::SetupInputComponent: PauseAction is invalid!"));
	}
}

void AShapeShifterPlayerController::OnPause()
{
	OnSetPause(true);
}

void AShapeShifterPlayerController::OnUnpause()
{
	OnSetPause(false);
}

void AShapeShifterPlayerController::OnSetPause(bool bPaused)
{
	if (!GetWorld() || !GetWorld()->GetAuthGameMode())
	{
		return;
	}

	bShowMouseCursor = bPaused;

	UGameViewportClient* GameViewportClient = GetLocalPlayer()->ViewportClient;

	if (GameViewportClient)
	{
		AShapeShifterHUD* CurrentHUD = Cast<AShapeShifterHUD>(GameViewportClient->GetGameInstance()
			->GetPrimaryPlayerController()->GetHUD());

		if (CurrentHUD)
		{
			bPaused ? CurrentHUD->OpenPauseMenu() : CurrentHUD->ClosePauseMenu();
		}
	}

	if (bPaused)
	{
		SetInputMode(FInputModeUIOnly());
		GetWorld()->GetAuthGameMode()->SetPause(this);
	}
	else
	{
		SetInputMode(FInputModeGameOnly());
		GetWorld()->GetAuthGameMode()->ClearPause();
	}
}