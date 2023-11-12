// Fill out your copyright notice in the Description page of Project Settings.

#include "Pawns/BallPawn.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Common/Enums/BallPawnForm.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "BuoyancyComponent.h"
#include "Actors/SaveGameManager.h"
#include "GameModes/ShapeShifterGameMode.h"
#include "Objects/ShapeShifterSaveGame.h"

ABallPawn::ABallPawn()
{
	SetupComponents();

	CurrentForm = EBallPawnForm::Rubber;

	FormMaterials.Add(EBallPawnForm::Rubber);
	FormMaterials.Add(EBallPawnForm::Metal);

	FormPhysicalMaterials.Add(EBallPawnForm::Rubber);
	FormPhysicalMaterials.Add(EBallPawnForm::Metal);

	FormMasses.Add(EBallPawnForm::Rubber, 1);
	FormMasses.Add(EBallPawnForm::Metal, 2);
}

void ABallPawn::SetupComponents()
{
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Sphere"));
	RootComponent = MeshComponent;

	MeshComponent->SetCollisionProfileName(TEXT("Pawn"));
	MeshComponent->bMultiBodyOverlap = true;

	MeshComponent->SetSimulatePhysics(true);

	// Enable Hit Events
	MeshComponent->SetNotifyRigidBodyCollision(true);

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArmComponent->SetupAttachment(MeshComponent);

	SpringArmComponent->bUsePawnControlRotation = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArmComponent);

	BuoyancyComponent = CreateDefaultSubobject<UBuoyancyComponent>(TEXT("Buoyancy"));
}

void ABallPawn::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

// This is just for preview in editor. We don't need to set it twice in packaged project.
#if WITH_EDITOR
	// Call SetForm in OnConstruction to preview CurrentForm
	SetForm(CurrentForm);
#endif
}

UStaticMeshComponent* ABallPawn::GetMesh() const
{
	return MeshComponent;
}

void ABallPawn::BeginPlay()
{
	Super::BeginPlay();

	InitDefaultMappingContext();
	InitWaterFluidSimulation();

	// Call SetForm with CurrentForm to apply everything related to it
	SetForm(CurrentForm);

	SetupSaveLoadDelegates();
}

void ABallPawn::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorldTimerManager().ClearTimer(CreateCloneTimer);
	GetWorldTimerManager().ClearTimer(JumpOnWaterSurfaceTimer);
}

void ABallPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	LastUpdateVelocity = GetVelocity();
}

void ABallPawn::SetupSaveLoadDelegates()
{
	// Don't manage save/load for Clone
	if (!IsPlayerControlled())
	{
		return;
	}

	const AShapeShifterGameMode* GameMode = GetWorld()->GetAuthGameMode<AShapeShifterGameMode>();

	if (!IsValid(GameMode))
	{
		UE_LOG(LogTemp, Error, TEXT("ABallPawn::SetupSaveLoadDelegates: Failed to get GameMode!"));

		return;
	}

	SaveGameManager = GameMode->GetSaveGameManager();

	if (!SaveGameManager.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("ABallPawn::SetupSaveLoadDelegates: Failed to get SaveGameManager!"));

		return;
	}

	SaveGameManager->OnSaveGame.AddDynamic(this, &ABallPawn::OnSaveGame);
	SaveGameManager->OnLoadGame.AddDynamic(this, &ABallPawn::OnLoadGame);
}

void ABallPawn::OnSaveGame(UShapeShifterSaveGame* SaveGameObject)
{
	if (!IsValid(SaveGameObject))
	{
		UE_LOG(LogTemp, Error, TEXT("ABallPawn::OnSaveGame: SaveGameObject is invalid!"));

		return;
	}

	// Save player variables
	SaveGameObject->BallPawnSaveData.PlayerTransform = GetActorTransform();
	SaveGameObject->BallPawnSaveData.PlayerVelocity = GetVelocity();
	SaveGameObject->BallPawnSaveData.PlayerForm = CurrentForm;

	// Don't save Clone variables if it doesn't exists
	if (!Clone.IsValid())
	{
		// Save that we don't have Clone
		SaveGameObject->BallPawnSaveData.bHasPlayerClone = false;

		return;
	}

	// Save that we have Clone and its variables in another case
	SaveGameObject->BallPawnSaveData.bHasPlayerClone = true;
	SaveGameObject->BallPawnSaveData.CloneTransform = Clone->GetActorTransform();
	SaveGameObject->BallPawnSaveData.CloneVelocity = Clone->GetVelocity();
}

void ABallPawn::OnLoadGame(UShapeShifterSaveGame* SaveGameObject)
{
	if (!IsValid(SaveGameObject))
	{
		UE_LOG(LogTemp, Error, TEXT("ABallPawn::OnLoadGame: SaveGameObject is invalid!"));

		return;
	}

	// Load player variables
	SetActorTransform(SaveGameObject->BallPawnSaveData.PlayerTransform);
	MeshComponent->SetAllPhysicsLinearVelocity(SaveGameObject->BallPawnSaveData.PlayerVelocity);
	SetForm(SaveGameObject->BallPawnSaveData.PlayerForm);

	// Don't load Clone variables if bHasPlayerClone is false and destroy it if it's already exists
	if (!SaveGameObject->BallPawnSaveData.bHasPlayerClone)
	{
		if (Clone.IsValid())
		{
			Clone->Destroy();
		}

		return;
	}

	// Spawn Clone in another case and if wasn't spawned before  
	if (!Clone.IsValid())
	{
		SpawnClone();

		// Return if failed to spawn Clone
		if (!Clone.IsValid())
		{
			UE_LOG(LogTemp, Error, TEXT("ABallPawn::OnLoadGame: Failed to spawn clone!"));

			return;
		}
	}

	// Load Clone variables
	Clone->SetActorTransform(SaveGameObject->BallPawnSaveData.CloneTransform);
	Clone->MeshComponent->SetAllPhysicsLinearVelocity(SaveGameObject->BallPawnSaveData.CloneVelocity);
}

void ABallPawn::InitDefaultMappingContext() const
{
	// Get PlayerController to get LocalPlayer
	const APlayerController* PlayerController = GetController<APlayerController>();

	if (!IsValid(PlayerController))
	{
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
	UEnhancedInputLocalPlayerSubsystem* LocalPlayerSubsystem = LocalPlayer->GetSubsystem<
		UEnhancedInputLocalPlayerSubsystem>();

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
		UE_LOG(LogTemp, Error,
			TEXT("ABallPawn::SetupPlayerInputComponent: InputComponentClass must be EnhancedInputComponent!"));

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
		EnhancedInputComponent->BindAction(ChangeFormAction, ETriggerEvent::Triggered, this,
			&ABallPawn::ChangeForm);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ABallPawn::SetupPlayerInputComponent: ChangeFormAction is invalid!"));
	}

	if (IsValid(CreateCloneAction))
	{
		EnhancedInputComponent->BindAction(CreateCloneAction, ETriggerEvent::Triggered, this,
			&ABallPawn::CreateClone);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ABallPawn::SetupPlayerInputComponent: CreateCloneAction is invalid!"));
	}

	if (IsValid(SaveGameAction))
	{
		EnhancedInputComponent->BindAction(SaveGameAction, ETriggerEvent::Triggered, this,
			&ABallPawn::SaveGame);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ABallPawn::SetupPlayerInputComponent: SaveGameAction is invalid!"));
	}

	if (IsValid(LoadGameAction))
	{
		EnhancedInputComponent->BindAction(LoadGameAction, ETriggerEvent::Triggered, this,
			&ABallPawn::LoadGame);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ABallPawn::SetupPlayerInputComponent: LoadGameAction is invalid!"));
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
	// Check if we're ever able to jump. Return if not.
	if (!bCanEverJump)
	{
		return;
	}

	// Disable jumping if we're falling and if we're not swimming on water surface
	if (IsFalling() && !IsSwimmingOnWaterSurface())
	{
		bCanJump = false;
	}

	// We're doing it here to be able to jump if BallPawn is swimming but didn't has enough force to jump from water
	if (bOverlappingWaterJumpZone)
	{
		EnableJumpIfSwimmingWithDelay();
	}

	// Return if we can't jump
	if (!bCanJump)
	{
		return;
	}

	// Add Z impulse to MeshComponent
	MeshComponent->AddImpulse(FVector(0, 0, JumpImpulse));

	// Disable jumping until we landed
	bCanJump = false;
}

bool ABallPawn::IsFalling() const
{
	// Get Mesh bounds
	FVector MeshCenter = MeshComponent->Bounds.Origin;
	float MeshRadius = MeshComponent->Bounds.SphereRadius;

	const float MaxDistanceToGround = 3;

	// Calculate MeshBottomPoint
	FVector MeshBottomPoint = MeshCenter - FVector(0, 0, MeshRadius);

	// Calculate TraceEnd with MaxDistanceToGround
	FVector TraceEnd = MeshBottomPoint - FVector(0, 0, MaxDistanceToGround);

	// Create out HitResult
	FHitResult HitResult;

	// Ignore this Actor in LineTrace
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	// LineTrace from MeshComponent to ground to check if we're touching ground and return false if we do
	return !GetWorld()->LineTraceSingleByChannel(HitResult, MeshCenter, TraceEnd, FallingCollisionTraceChanel,
		Params);
}

void ABallPawn::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved,
	FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	// Calculate HitImpulse (impact impulse) by subtracting LastUpdateVelocity and current velocity
	const FVector HitImpulse = LastUpdateVelocity - GetVelocity();

	// Maximum HitImpulse.Z tolerance to allow jumping
	const float Tolerance = 1;

	// Enable jumping if we're not falling and HitImpulse.Z is in range of Tolerance
	if (!IsFalling() && HitImpulse.Z >= -Tolerance && HitImpulse.Z <= Tolerance)
	{
		bCanJump = true;
	}
}

void ABallPawn::EnableJumpIfSwimmingWithDelay()
{
	// Don't do anything if timer was already set
	if (GetWorldTimerManager().IsTimerActive(JumpOnWaterSurfaceTimer))
	{
		return;
	}

	// Set JumpOnWaterSurfaceTimer with JumpOnWaterSurfaceDelay. Enable jumping if swimming on execute.
	GetWorldTimerManager().SetTimer(JumpOnWaterSurfaceTimer, [this]()
	{
		if (IsSwimmingOnWaterSurface())
		{
			bCanJump = true;
		}
	}, JumpOnWaterSurfaceDelay, false);
}

EBallPawnForm ABallPawn::GetForm() const
{
	return CurrentForm;
}

void ABallPawn::SetForm(const EBallPawnForm NewForm)
{
	// Destroy clone if NewForm isn't same as previous one and bDestroyCloneOnChangeForm is true
	if (CurrentForm != NewForm && bDestroyCloneOnChangeForm)
	{
		// Clear CreateCloneTimer to cancel Clone creation if timer has been set
		GetWorldTimerManager().ClearTimer(CreateCloneTimer);

		// Destroy Clone if it was created
		if (Clone.IsValid())
		{
			Clone->Destroy();
		}
	}

	CurrentForm = NewForm;

	// Allow Laser reflection if CurrentForm is Metal
	if (CurrentForm == EBallPawnForm::Metal)
	{
		Tags.AddUnique(ReflectLaserTagName);
	}
	// Forbid Laser reflection in another case
	else
	{
		Tags.Remove(ReflectLaserTagName);
	}

	/*
	 * Find Material associated with NewForm in FormMaterials.
	 * We call FindRef instead of Find to avoid pointer to pointer.
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
		UE_LOG(LogTemp, Warning,
			TEXT("ABallPawn::SetForm: Failed to find Material associated with NewForm in FormMaterials"));
	}

	/**
	 * Find PhysicalMaterial associated with NewForm in FormPhysicalMaterials.
	 * We call FindRef instead of Find to avoid pointer to pointer.
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
		UE_LOG(LogTemp, Warning, TEXT("ABallPawn::SetForm: Failed to find PhysicalMaterial associated with NewForm in "
			"FormPhysicalMaterials"));
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

	// Find Buoyancy associated with NewForm in FormBuoyancyData
	const FBuoyancyData* BuoyancyData = FormBuoyancyData.Find(NewForm);

	/**
	 * The next functionality will be called only if we already begun play to avoid crash in editor.
	 * We don't need to check it in packaged project.
	 */
#if WITH_EDITOR
	if (!HasActorBegunPlay())
	{
		return;
	}
#endif

	// Disable jumping if BallPawn is overlapping water zone but he isn't in Rubber form
	if (bOverlappingWaterJumpZone)
	{
		if (CurrentForm != EBallPawnForm::Rubber)
		{
			bCanJump = false;
		}

		// Enable jumping back if swimming on water surface, but with delay to avoid jump spamming
		EnableJumpIfSwimmingWithDelay();
	}

	// Set BuoyancyData as BuoyancyComponent BuoyancyData if it's valid
	if (BuoyancyData)
	{
		BuoyancyComponent->BuoyancyData = *BuoyancyData;
	}
	// Send Warning log in another case
	else
	{
		UE_LOG(LogTemp, Warning,
			TEXT("ABallPawn::SetForm: Failed to find BuoyancyData associated with NewForm in FormBuoyancyData"));
	}
}

void ABallPawn::ChangeForm()
{
	// Check if we're ever able to change form. Return if not.
	if (!bCanEverChangeForm)
	{
		return;
	}

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

void ABallPawn::CreateClone()
{
	// Clear CreateCloneTimer to avoid multiple clone creation by CreateClone call spamming
	GetWorldTimerManager().ClearTimer(CreateCloneTimer);

	// Remember current Actor Transform where clone will be spawned
	CloneSpawnTransform = GetActorTransform();

	// Call SpawnClone in CreateCloneRate seconds
	GetWorldTimerManager().SetTimer(CreateCloneTimer, this, &ABallPawn::SpawnClone, CreateCloneRate);
}

void ABallPawn::SpawnClone()
{
	// Check if we're ever able to create clone. Return if not.
	if (!bCanEverCreateClone)
	{
		return;
	}

	// Destroy old Clone if it was created before
	if (Clone.IsValid())
	{
		Clone->Destroy();
	}

	// Check if we can spawn Clone and return if not
	if (!CanSpawnClone())
	{
		UE_LOG(LogTemp, Display,
			TEXT("ABallPawn::SpawnClone: Unable to spawn Clone. Clone is colliding with player."));

		return;
	}

	FActorSpawnParameters SpawnParameters;

	// We have to set Clone scale same as players instead of multiplying them by each other
	SpawnParameters.TransformScaleMethod = ESpawnActorScaleMethod::OverrideRootScale;

	// We have to always spawn clone because we have own check for colliding
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// Spawn Clone
	Clone = GetWorld()->SpawnActor<ABallPawn>(GetClass(), CloneSpawnTransform, SpawnParameters);

	// Set same Form to NewClone as ours but check if Clone is valid just in case
	if (Clone.IsValid())
	{
		Clone->SetForm(CurrentForm);
	}
}

bool ABallPawn::CanSpawnClone() const
{
	// Get CloneLocation from CloneSpawnTransform
	const FVector CloneLocation = CloneSpawnTransform.GetLocation();

	// Get CloneRadius from MeshComponent Bounds
	const float CloneRadius = MeshComponent->Bounds.SphereRadius;

	// Unused HitResult for sphere trace
	FHitResult HitResult;

	// Do sphere trace by Pawn collision channel to check if Clone will collide player. Return false if colliding.
	return !GetWorld()->SweepSingleByChannel(HitResult, CloneLocation, CloneLocation,
		CloneSpawnTransform.GetRotation(), SpawnCloneCheckTraceChanel, FCollisionShape::MakeSphere(CloneRadius));
}

// ReSharper disable once CppMemberFunctionMayBeConst
void ABallPawn::SaveGame()
{
	if (SaveGameManager.IsValid())
	{
		SaveGameManager->SaveGame();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ABallPawn::SaveGame: SaveGameManager is invalid!"));
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void ABallPawn::LoadGame()
{
	if (SaveGameManager.IsValid())
	{
		SaveGameManager->LoadGame();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ABallPawn::LoadGame: SaveGameManager is invalid!"));
	}
}

void ABallPawn::InitWaterFluidSimulation()
{
	if (!WaterFluidSimulationClass)
	{
		UE_LOG(LogTemp, Error, TEXT("ABallPawn::InitWaterFluidSimulation: WaterFluidSimulationClass is invalid!"));

		return;
	}

	// Get all WaterFluidActors on scene
	TArray<AActor*> WaterFluidActors;
	UGameplayStatics::GetAllActorsOfClass(this, WaterFluidSimulationClass, WaterFluidActors);

	// Call RegisterDynamicForce for every water fluid Actor
	for (AActor* It : WaterFluidActors)
	{
		RegisterDynamicForce(It, MeshComponent, MeshComponent->Bounds.SphereRadius, WaterFluidForceStrength);
	}
}

bool ABallPawn::IsSwimmingOnWaterSurface() const
{
	return bOverlappingWaterJumpZone && CurrentForm == EBallPawnForm::Rubber;
}

void ABallPawn::SetOverlappingWaterJumpZone(const bool bNewOverlappingWaterJumpZone)
{
	// Don't do anything if current and new states are same
	if (bOverlappingWaterJumpZone == bNewOverlappingWaterJumpZone)
	{
		return;
	}

	bOverlappingWaterJumpZone = bNewOverlappingWaterJumpZone;

	// Try to enable jumping if we're overlapping water jump zone
	if (bOverlappingWaterJumpZone)
	{
		EnableJumpIfSwimmingWithDelay();
	}
}