// Fill out your copyright notice in the Description page of Project Settings.

#include "Pawns/BallPawn.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

ABallPawn::ABallPawn()
{
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Sphere"));
	RootComponent = MeshComponent;

	MeshComponent->SetSimulatePhysics(true);

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArmComponent->SetupAttachment(MeshComponent);

	SpringArmComponent->bUsePawnControlRotation = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArmComponent);
}

void ABallPawn::BeginPlay()
{
	Super::BeginPlay();

	InitDefaultMappingContext();

	// Call LimitViewPitch with default values
	LimitViewPitch(DefaultMinViewPitch, DefaultMaxViewPitch);
}

void ABallPawn::LimitViewPitch(const float MinViewPitch, const float MaxViewPitch) const
{
	const APlayerController* PlayerController = GetController<APlayerController>();

	if (!IsValid(PlayerController))
	{
		UE_LOG(LogTemp, Error, TEXT("ABallPawn::LimitViewPitch: PlayerController is invalid!"));
	}

	APlayerCameraManager* PlayerCameraManager = PlayerController->PlayerCameraManager;

	if (!IsValid(PlayerCameraManager))
	{
		UE_LOG(LogTemp, Error, TEXT("ABallPawn::LimitViewPitch: PlayerCameraManager is invalid!"));
	}

	PlayerCameraManager->ViewPitchMin = MinViewPitch;
	PlayerCameraManager->ViewPitchMax = MaxViewPitch;
}

void ABallPawn::InitDefaultMappingContext() const
{
	// Get PlayerController to get LocalPlayer
	const APlayerController* PlayerController = GetController<APlayerController>();

	if (!IsValid(PlayerController))
	{
		UE_LOG(LogTemp, Error, TEXT("ABallPawn::InitDefaultMappingContext: PlayerController is invalid!"));

		return;
	}

	// Get LocalPlayer to get Subsystem
	const ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();

	if (!IsValid(LocalPlayer))
	{
		UE_LOG(LogTemp, Error, TEXT("ABallPawn::InitDefaultMappingContext: LocalPlayer is invalid!"));

		return;
	}

	// Get LocalPlayerSubsystem
	UEnhancedInputLocalPlayerSubsystem* LocalPlayerSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();

	if (!IsValid(LocalPlayerSubsystem))
	{
		UE_LOG(LogTemp, Error, TEXT("ABallPawn::InitDefaultMappingContext: Subsystem is invalid!"));

		return;
	}

	// Add DefaultMappingContext to LocalPlayerSubsystem if it wasn't added before
	if (!LocalPlayerSubsystem->HasMappingContext(DefaultMappingContext))
	{
		LocalPlayerSubsystem->AddMappingContext(DefaultMappingContext, 0);
	}
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

	if (IsValid(MoveAction))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABallPawn::Move);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ABallPawn::SetupPlayerInputComponent: MoveAction is invalid!"));
	}

	if (IsValid(LookAction))
	{
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABallPawn::Look);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ABallPawn::SetupPlayerInputComponent: MoveAction is invalid!"));
	}

	if (IsValid(JumpAction))
	{
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ABallPawn::Jump);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ABallPawn::SetupPlayerInputComponent: MoveAction is invalid!"));
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void ABallPawn::Move(const FInputActionValue& Value)
{
	const FVector2D MoveAxisValue = Value.Get<FVector2D>();

	// Forward and backward movement
	if (MoveAxisValue.Y != 0)
	{
		const FVector ForwardCameraDirection = CameraComponent->GetForwardVector();
		MeshComponent->AddForce(MoveAxisValue.Y * MovementSpeed * ForwardCameraDirection);

		UE_LOG(LogTemp, Display, TEXT("ForwardCameraDirection: %s"), *ForwardCameraDirection.ToString())
	}

	// Left and right movement
	if (MoveAxisValue.X != 0)
	{
		const FVector RightCameraDirection = CameraComponent->GetRightVector();
		MeshComponent->AddForce(MoveAxisValue.X * MovementSpeed * RightCameraDirection);
	}
}

void ABallPawn::Look(const FInputActionValue& Value)
{
	// Get LookAxisValue from Value
	const FVector2D LookAxisValue = Value.Get<FVector2D>();

	// X Camera rotation
	if (LookAxisValue.X != 0)
	{
		AddControllerYawInput(LookAxisValue.X);
	}

	// Y Camera rotation
	if (LookAxisValue.Y != 0)
	{
		AddControllerPitchInput(LookAxisValue.Y);
	}
}

void ABallPawn::Jump(const FInputActionValue& Value)
{
	// Check if Z velocity is clear
	if (!IsZVelocityClear())
	{
		bCanJump = false;
	}

	// Check if we can jump
	if (!bCanJump)
	{
		return;
	}

	// Add Z impulse to MeshComponent
	MeshComponent->AddImpulse(FVector(0, 0, JumpImpulse));

	// Disable jumping until we landed
	bCanJump = false;
}

bool ABallPawn::IsZVelocityClear(const float Tolerance) const
{
	return GetVelocity().Z >= -Tolerance && GetVelocity().Z <= Tolerance;
}

void ABallPawn::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved,
	FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	// Enable Jump if we landed
	if (IsZVelocityClear())
	{
		bCanJump = true;
	}
}
