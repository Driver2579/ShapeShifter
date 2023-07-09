// Fill out your copyright notice in the Description page of Project Settings.

#include "Pawns/BallPawn.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

ABallPawn::ABallPawn()
{
	SetupComponents();

	FormMasses.Add(EBallPawnForm::Rubber, 1);
	FormMasses.Add(EBallPawnForm::Metal, 2);
}

void ABallPawn::SetupComponents()
{
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Sphere"));
	RootComponent = MeshComponent;

	MeshComponent->SetRelativeLocation(FVector::ZeroVector);

	MeshComponent->SetSimulatePhysics(true);

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArmComponent->SetupAttachment(MeshComponent);

	SpringArmComponent->bUsePawnControlRotation = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArmComponent);
}

void ABallPawn::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	// Call SetForm in OnConstruction to preview CurrentForm and make it work after BeginPlay
	SetForm(CurrentForm);
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
		UE_LOG(LogTemp, Error, TEXT("ABallPawn::SetupPlayerInputComponent: LookAction is invalid!"));
	}

	if (IsValid(JumpAction))
	{
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ABallPawn::Jump);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ABallPawn::SetupPlayerInputComponent: JumpAction is invalid!"));
	}

	if (IsValid(ChangeFormAction))
	{
		EnhancedInputComponent->BindAction(ChangeFormAction, ETriggerEvent::Triggered, this, &ABallPawn::ChangeForm);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ABallPawn::SetupPlayerInputComponent: ChangeFormAction is invalid!"));
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void ABallPawn::Move(const FInputActionValue& Value)
{
	const FVector2D MoveAxisValue = Value.Get<FVector2D>();

	// Get Yaw of ControlRotation and store it in FRotator with 0 Pitch and Roll
	const FRotator YawControlRotation = FRotator(0, GetControlRotation().Yaw, 0);

	// Forward and backward movement
	if (MoveAxisValue.Y != 0)
	{
		// Get ForwardControlDirection (Y FRotator axis). X - FVector axis
		const FVector ForwardControlDirection = FRotationMatrix(YawControlRotation).GetUnitAxis(EAxis::X);

		// Add force to MeshComponent with MoveAxisVale.Y multiplied with MovementSpeed and direction to move
		MeshComponent->AddForce(MoveAxisValue.Y * MovementSpeed * ForwardControlDirection);
	}

	// Left and right movement
	if (MoveAxisValue.X != 0)
	{
		// Get RightControlDirection (X FRotator axis). Y - FVector axis
		const FVector RightControlDirection = FRotationMatrix(YawControlRotation).GetUnitAxis(EAxis::Y);

		// Add force to MeshComponent with MoveAxisVale.X multiplied with MovementSpeed and direction to move
		MeshComponent->AddForce(MoveAxisValue.X * MovementSpeed * RightControlDirection);
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

void ABallPawn::SetForm(const EBallPawnForm NewForm)
{
	CurrentForm = NewForm;

	/*
	 * Find Material associated with NewForm in FormMaterials.
	 * We call FindRef instead of Find to avoid pointer to pointer
	 */
	UMaterial* FormMaterial = FormMaterials.FindRef(NewForm);

	// Set FormMaterial as MeshComponent Material if it's valid
	if (IsValid(FormMaterial))
	{
		MeshComponent->SetMaterial(0, FormMaterial);
	}
	// Send Warning log in another case
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ABallPawn::SetForm: Failed to find Material associated with NewForm in FormMaterials"));
	}

	/*
	 * Find PhysicalMaterial associated with NewForm in FormPhysicalMaterials.
	 * We call FindRef instead of Find to avoid pointer to pointer
	 */
	UPhysicalMaterial* FormPhysicalMaterial = FormPhysicalMaterials.FindRef(NewForm);

	// Set FormPhysicalMaterial as MeshComponent PhysicalMaterial if it's valid
	if (IsValid(FormPhysicalMaterial))
	{
		MeshComponent->SetPhysMaterialOverride(FormPhysicalMaterial);
	}
	// Send Warning log in another case
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ABallPawn::SetForm: Failed to find PhysicalMaterial associated with NewForm in FormPhysicalMaterials"));
	}

	// Find Mass associated with NewForm in FormMasses
	const float* FormMass = FormMasses.Find(NewForm);

	// Set FormMass as MeshComponent Mass if it's valid
	if (FormMass)
	{
		MeshComponent->SetMassOverrideInKg(NAME_None, *FormMass);
	}
	// Send Warning log in another case
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ABallPawn::SetForm: Failed to find Mass associated with NewForm in FormMasses"));
	}
}

void ABallPawn::ChangeForm()
{
	switch (CurrentForm)
	{
	// Set Metal form if CurrentForm is Rubber
	case EBallPawnForm::Rubber:
		SetForm(EBallPawnForm::Metal);

		break;

	// Set Rubber form if CurrentForm is Metal
	case EBallPawnForm::Metal:
		SetForm(EBallPawnForm::Rubber);

		break;
	}
}