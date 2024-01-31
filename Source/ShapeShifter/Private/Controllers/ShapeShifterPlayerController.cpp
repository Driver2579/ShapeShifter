// Fill out your copyright notice in the Description page of Project Settings.

#include "Controllers/ShapeShifterPlayerController.h"

#include "UI/HUDs/ShapeShifterHUD.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/GameModeBase.h"

void AShapeShifterPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Get LocalPlayer to get its subsystem
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

	/**
	* When entering from the level where there was control through the cursor, it will remain visible. To fix this, you
	* need this line.
	*/
	SetInputMode(FInputModeGameOnly());

	if (!IsValid(GetLocalPlayer()))
	{
		UE_LOG(LogTemp, Error, TEXT("AShapeShifterPlayerController::BeginPlay: LocalPlayer is invalid!"));

		return;
	}

	// Get GameViewportClient to get GameInstance
	const UGameViewportClient* GameViewportClient = GetLocalPlayer()->ViewportClient;

	if (!IsValid(GameViewportClient))
	{
		UE_LOG(LogTemp, Error, TEXT("AShapeShifterPlayerController::BeginPlay: GameViewportClient is invalid!"));

		return;
	}

	// Get GameInstance to get PrimaryPlayerController
	const UGameInstance* GameInstance = GameViewportClient->GetGameInstance();

	if (!IsValid(GameInstance))
	{
		UE_LOG(LogTemp, Error, TEXT("AShapeShifterPlayerController::BeginPlay: GameInstance is invalid!"));

		return;
	}

	// Get PrimaryPlayerController to get CurrentHUD
	const APlayerController* PrimaryPlayerController = GameInstance->GetPrimaryPlayerController();

	if (!IsValid(PrimaryPlayerController))
	{
		UE_LOG(LogTemp, Error, TEXT("AShapeShifterPlayerController::BeginPlay: PrimaryPlayerController is invalid!"));

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
			&AShapeShifterPlayerController::Pause);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("AShapeShifterPlayerController::SetupInputComponent: PauseAction is invalid!"));
	}
}

void AShapeShifterPlayerController::Pause()
{
	if (!CurrentHUD.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("AShapeShifterPlayerController::SetPause: CurrentHUD is invalid!"));

		return;
	}

	// Open pause menu
	CurrentHUD->OpenPauseMenu();

	if (!IsValid(GetWorld()->GetAuthGameMode()))
	{
		UE_LOG(LogTemp, Error, TEXT("AShapeShifterPlayerController::SetPause: GameMode is invalid!"));

		return;
	}

	// Pause game
	SetInputMode(FInputModeUIOnly());
	GetWorld()->GetAuthGameMode()->SetPause(this);

	// Show cursor
	bShowMouseCursor = true;
}

void AShapeShifterPlayerController::Unpause()
{
	if (!CurrentHUD.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("AShapeShifterPlayerController::SetPause: CurrentHUD is invalid!"));

		return;
	}

	// Close pause menu
	CurrentHUD->ClosePauseMenu();

	if (!IsValid(GetWorld()->GetAuthGameMode()))
	{
		UE_LOG(LogTemp, Error, TEXT("AShapeShifterPlayerController::SetPause: GameMode is invalid!"));

		return;
	}
	
	// Unpause game
	SetInputMode(FInputModeGameOnly());
	GetWorld()->GetAuthGameMode()->ClearPause();

	// Hide cursor
	bShowMouseCursor = false;

	
	SetPause(false);
}