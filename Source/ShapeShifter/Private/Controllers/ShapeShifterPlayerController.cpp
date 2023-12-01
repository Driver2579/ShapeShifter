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
		UE_LOG(LogTemp, Error, TEXT("AShapeShifterPlayerController::BeginPlay: LocalPlayer is invalid!"));

		return;
	}

	// Get LocalPlayerSubsystem to add DefaultMappingContext
	UEnhancedInputLocalPlayerSubsystem* LocalPlayerSubsystem = LocalPlayer->GetSubsystem<
		UEnhancedInputLocalPlayerSubsystem>();

	if (!IsValid(LocalPlayerSubsystem))
	{
		UE_LOG(LogTemp, Error, TEXT("AShapeShifterPlayerController::BeginPlay: Subsystem is invalid!"));

		return;
	}

	// Add DefaultMappingContext to LocalPlayerSubsystem if it wasn't added before
	if (!LocalPlayerSubsystem->HasMappingContext(ShapeShifterMappingContext))
	{
		LocalPlayerSubsystem->AddMappingContext(ShapeShifterMappingContext, 0);
	}

	SetInputMode(FInputModeGameOnly());

	if (!IsValid(GetLocalPlayer()))
	{
		UE_LOG(LogTemp, Error, TEXT("AShapeShifterPlayerController::SetPause: LocalPlayer is invalid!"));

		return;
	}

	// Get GameViewportClient to get GameInstance
	const UGameViewportClient* GameViewportClient = GetLocalPlayer()->ViewportClient;

	if (!IsValid(GameViewportClient))
	{
		UE_LOG(LogTemp, Error, TEXT("AShapeShifterPlayerController::SetPause: GameViewportClient is invalid!"));

		return;
	}

	// Get GameInstance to get PrimaryPlayerController
	const UGameInstance* GameInstance = GameViewportClient->GetGameInstance();

	if (!IsValid(GameInstance))
	{
		UE_LOG(LogTemp, Error, TEXT("AShapeShifterPlayerController::SetPause: GameInstance is invalid!"));

		return;
	}

	// Get PrimaryPlayerController to get CurrentHUD
	const APlayerController* PrimaryPlayerController = GameInstance->GetPrimaryPlayerController();

	if (!IsValid(PrimaryPlayerController))
	{
		UE_LOG(LogTemp, Error, TEXT("AShapeShifterPlayerController::SetPause: PrimaryPlayerController is invalid!"));

		return;
	}

	// Save CurrentHUD
	CurrentHUD = Cast<AShapeShifterHUD>(PrimaryPlayerController->GetHUD());
}

void AShapeShifterPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);

	if (!IsValid(EnhancedInputComponent))
	{
		UE_LOG(LogTemp, Error, TEXT(
			"AShapeShifterPlayerController::SetupInputComponent: EnhancedInputComponent is invalid!"));

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
	SetPause(true);
}

void AShapeShifterPlayerController::OnUnpause()
{
	SetPause(false);
}

void AShapeShifterPlayerController::SetPause(bool bPaused)
{
	if (!CurrentHUD.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("AShapeShifterPlayerController::SetPause: CurrentHUD is invalid!"));

		return;
	}

	// Open or close menu
	bPaused ? CurrentHUD->OpenPauseMenu() : CurrentHUD->ClosePauseMenu();

	if (!IsValid(GetWorld()->GetAuthGameMode()))
	{
		UE_LOG(LogTemp, Error, TEXT("AShapeShifterPlayerController::SetPause: GameMode is invalid!"));

		return;
	}

	// Pause game
	if (bPaused)
	{
		SetInputMode(FInputModeUIOnly());
		GetWorld()->GetAuthGameMode()->SetPause(this);
	}
	// Unpause game
	else
	{
		SetInputMode(FInputModeGameOnly());
		GetWorld()->GetAuthGameMode()->ClearPause();
	}

	// Show cursor if menu is open or hide if it is close
	bShowMouseCursor = bPaused;
}