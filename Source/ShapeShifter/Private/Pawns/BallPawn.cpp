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
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Actors/SaveGameManager.h"
#include "Objects/ShapeShifterSaveGame.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"

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
	
	RollingAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("Rolling Audio"));
	RollingAudioComponent->SetupAttachment(RootComponent);

	AirSlicingAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("Air Slicing Audio"));
	AirSlicingAudioComponent->SetupAttachment(RootComponent);
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

	// This sound needs to play all the time, but it's pitch and volume can change during runtime
	RollingAudioComponent->Play();
}

void ABallPawn::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	// Clear all timers
	GetWorldTimerManager().ClearTimer(CreateCloneTimer);
	GetWorldTimerManager().ClearTimer(JumpOnWaterSurfaceTimer);
	GetWorldTimerManager().ClearTimer(LoadAfterDeathTimer);
}

void ABallPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	LastUpdateVelocity = GetRootComponent()->GetComponentVelocity();
	
	// How much velocity is greater than the minimum velocity for sound
	const float HowMuchMoreThanMinAirSlicing = FMath::Max(GetVelocity().Length() - MinVelocityAirSlicingSound, 0);
	
	// Range to set pitch and volume to AirSlicingAudioComponent
	const float AlphaVelocityAirSlicing = FMath::Min(HowMuchMoreThanMinAirSlicing / MaxVelocityAirSlicingSound, 1);
	
	// Pitch should be from MinPitchAirSlicingSound to MaxPitchAirSlicingSound at AlphaVelocityAirSlicing
	// TODO: Replace it with:
	// AirSlicingAudioComponent->SetPitchMultiplier(FMath::Lerp(MinPitchAirSlicingSound, MaxPitchAirSlicingSound,
	//		AlphaVelocityAirSlicingSound));
	AirSlicingAudioComponent->SetPitchMultiplier(FMath::Lerp(MinPitchAirSlicingSound,1, AlphaVelocityAirSlicing));

	// Volume should be from 0 to 1 at AlphaVelocityAirSlicing
	AirSlicingAudioComponent->SetVolumeMultiplier(FMath::Lerp(0, 1, AlphaVelocityAirSlicing));
	
	// Rolling sound cannot be heard if the ball is in the air
	if (IsFalling())
	{
		RollingAudioComponent->SetVolumeMultiplier(0);

		return;
	}

	const FVector ProjectedVector = FVector::DotProduct(GetVelocity(), FVector(1, 1, 0)) *
		FVector(1, 1, 0);
	
	// Calculate the rolling sound volume
	RollingAudioComponent->SetVolumeMultiplier(FMath::Min(ProjectedVector.Length() / MaxVelocityRollingSound,
		MaxVelocityRollingSound));
	
	// Range to set pitch to AirSlicingAudioComponent
	const float AlphaRolling = ProjectedVector.Length() / MaxVelocityRollingSound;
	
	// Volume should be from MinPitchRollingSound to MaxPitchRollingSound at AlphaPitchRolling
	RollingAudioComponent->SetPitchMultiplier(
		FMath::Lerp(MinPitchRollingSound, MaxPitchRollingSound, AlphaRolling));
}

void ABallPawn::OnSavableSetup(ASaveGameManager* SaveGameManager)
{
	SaveGameManagerPtr = SaveGameManager;
}

void ABallPawn::OnSaveGame(UShapeShifterSaveGame* SaveGameObject)
{
	// Don't manage saving for the Clone
	if (!IsPlayerControlled())
	{
		return;
	}

	if (!IsValid(SaveGameObject))
	{
		UE_LOG(LogTemp, Error, TEXT("ABallPawn::OnSaveGame: SaveGameObject is invalid!"));

		return;
	}

	// Save player variables
	SaveGameObject->BallPawnSaveData.CameraRotation = GetControlRotation();
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
	// Don't manage loading for the Clone
	if (!IsPlayerControlled())
	{
		return;
	}

	if (!IsValid(SaveGameObject))
	{
		UE_LOG(LogTemp, Error, TEXT("ABallPawn::OnLoadGame: SaveGameObject is invalid!"));

		return;
	}

	// Revive the player if he was dead before loading
	if (bDead)
	{
		Revive();
	}

	APlayerController* PlayerController = GetController<APlayerController>();

	// Load CameraRotation if PlayerController is valid
	if (IsValid(PlayerController))
	{
		PlayerController->SetControlRotation(SaveGameObject->BallPawnSaveData.CameraRotation);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ABallPawn::OnLoadGame: PlayerController is invalid!"));
	}

	// Set location in PlayerTransform a little upper to avoid colliding with moving objects by Z axis
	FTransform& PlayerTransform = SaveGameObject->BallPawnSaveData.PlayerTransform;
	PlayerTransform.SetLocation(PlayerTransform.GetLocation() + FVector(0, 0, 1));

	// Load other player variables
	SetActorTransform(PlayerTransform);
	MeshComponent->SetPhysicsLinearVelocity(SaveGameObject->BallPawnSaveData.PlayerVelocity);
	SetForm(SaveGameObject->BallPawnSaveData.PlayerForm);
	PlayerController->SetControlRotation(SaveGameObject->BallPawnSaveData.CameraRotation);

	// Forget about creating clone on timer which has started before loading
	GetWorldTimerManager().ClearTimer(CreateCloneTimer);

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
		SpawnCloneObject();

		// Return if failed to spawn Clone
		if (!Clone.IsValid())
		{
			UE_LOG(LogTemp, Error, TEXT("ABallPawn::OnLoadGame: Failed to spawn clone!"));

			return;
		}
	}

	// Load Clone variables
	Clone->SetActorTransform(SaveGameObject->BallPawnSaveData.CloneTransform);
	Clone->MeshComponent->SetPhysicsLinearVelocity(SaveGameObject->BallPawnSaveData.CloneVelocity);
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

	if (!IsValid(JumpSound))
	{
		UE_LOG(LogTemp, Warning, TEXT("ABallPawn::Jump: JumpSound is invalid!"));

		return;
	}
	
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), JumpSound, GetActorLocation());
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

	// At what angle to the surface did the collision occur?
	const double ContactAngleRadians = FMath::Acos(FVector::DotProduct(HitNormal,
		GetVelocity().GetSafeNormal()));

	const double ContactAngleDegrees = FMath::RadiansToDegrees(ContactAngleRadians);
	
	if (AngelSound < ContactAngleDegrees || MinVelocityHitSound > GetVelocity().Length())
	{
		return;
	}
	
	if (!CurrentHitSound.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("ABallPawn::NotifyHit: CurrentHitSound is invalid!"));

		return;
	}

	// The hit volume depends on the velocity where the limit values are from MaxVelocityHitSound to MinVelocityHitSound
	const double HitVolume =  GetVelocity().Length() / (MaxVelocityHitSound - MinVelocityHitSound);
	
	const double LimitedHitVolume = FMath::Min(HitVolume, MaxVelocityHitSound);
	
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), CurrentHitSound.Get(), GetActorLocation(), LimitedHitVolume);
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
	// We have to spawn ChangeFormNiagaraComponent only if form was changed
	if (NewForm != CurrentForm)
	{
		FFXSystemSpawnParameters NiagaraSpawnParameters;

		// Initialize NiagaraSpawnParameters
		NiagaraSpawnParameters.SystemTemplate = ChangeFormNiagaraSystemTemplate;
		NiagaraSpawnParameters.AttachToComponent = RootComponent;
		NiagaraSpawnParameters.LocationType = EAttachLocation::SnapToTarget;

		// Spawn ChangeFormNiagaraComponent
		const UNiagaraComponent* ChangeFormNiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttachedWithParams(
			NiagaraSpawnParameters);

		if (!IsValid(ChangeFormNiagaraComponent))
		{
			UE_LOG(LogTemp, Warning, TEXT("ABallPawn::SetForm: Failed to spawn ChangeFormNiagaraComponent!"));
		}
	}

	// Remember OldForm before changing it to compare a new one with the old one when needed 
	const EBallPawnForm OldForm = CurrentForm;

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

	/**
	 * Destroy Clone if NewForm isn't same as previous one and bDestroyCloneOnChangeForm is true. We're doing it only
	 * after managing the ReflectionLaserTagName to avoid BallPawn death when changing to Metal form.
	 */
	if (NewForm != OldForm && bDestroyCloneOnChangeForm)
	{
		CancelCloneCreation();

		// Kill Clone if it was created
		if (Clone.IsValid())
		{
			Clone->Die();
		}
	}

	/*
	 * Find Material associated with NewForm in FormMaterials.
	 * We call FindRef instead of Find to avoid pointer to pointer.
	 */
	UMaterialInterface* FormMaterial = FormMaterials.FindRef(NewForm);

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

	if (CurrentForm == EBallPawnForm::Rubber)
	{
		CurrentHitSound = RubberHitSound;
		RollingAudioComponent->SetSound(RubberRollingSound);
	}
	else if (CurrentForm == EBallPawnForm::Metal)
	{
		CurrentHitSound = MetalHitSound;
		RollingAudioComponent->SetSound(MetalRollingSound);
	}

	if (!CurrentHitSound.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("ABallPawn::SetForm: CurrentHitSound is invalid!"));
	}

	if (!IsValid(RollingAudioComponent->Sound))
	{
		UE_LOG(LogTemp, Warning, TEXT("ABallPawn::SetForm: RollingSound is invalid!"));
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

	if (!IsValid(ChangeFormSound))
	{
		UE_LOG(LogTemp, Warning, TEXT("ABallPawn::ChangeForm: ChangeFormSound is invalid!"));
		
		return;
	}
	
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), ChangeFormSound, GetActorLocation());
}

void ABallPawn::CreateClone()
{
	// Only a player can create clone
	if (!IsPlayerControlled())
	{
		return;
	}

	CancelCloneCreation();

	// Spawn CreateCloneNiagaraComponent
	CreateCloneNiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(this,
		CreateCloneNiagaraSystemTemplate, GetActorLocation());

	if (!CreateCloneNiagaraComponent.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("ABallPawn::CreateClone: Failed to spawn CreateCloneNiagaraComponent!"));
	}

	// Remember current Actor Transform where clone will be spawned
	CloneSpawnTransform = GetActorTransform();

	// Call SpawnClone in CreateCloneRate seconds
	GetWorldTimerManager().SetTimer(CreateCloneTimer, this, &ABallPawn::SpawnClone, CreateCloneRate);
	
	if (!IsValid(CreateCloneSound))
	{
		UE_LOG(LogTemp, Warning, TEXT("ABallPawn::CreateClone: CreateCloneSound is invalid!"));
		
		return;
	}
	
	CreateCloneAudioComponent = UGameplayStatics::SpawnSoundAtLocation(GetWorld(), CreateCloneSound,
		GetActorLocation());
}

void ABallPawn::CancelCloneCreation()
{
	// There is nothing to cancel if CreateCloneTimer isn't active
	if (!GetWorldTimerManager().IsTimerActive(CreateCloneTimer))
	{
		return;
	}

	GetWorldTimerManager().ClearTimer(CreateCloneTimer);

	if (CreateCloneNiagaraComponent.IsValid())
	{
		CreateCloneNiagaraComponent->SetCustomTimeDilation(CreateCloneVfxSpeedOnCancel);
	}

	if (CreateCloneAudioComponent.IsValid())
	{
		CreateCloneAudioComponent->Stop();
	}

	if (!IsValid(CancelSpawnCloneSound))
	{
		UE_LOG(LogTemp, Warning, TEXT("ABallPawn::CancelCloneCreation: CancelSpawnCloneSound is invalid!"));
		
		return;
	}
	
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), CancelSpawnCloneSound, GetActorLocation());
}

void ABallPawn::SpawnClone()
{
	// Check if we're ever able to create clone. Return if not.
	if (!bCanEverCreateClone)
	{
		return;
	}

	// Kill old Clone if it was created before
	if (Clone.IsValid())
	{
		Clone->Die();
	}

	// Check if we can spawn Clone and return if not
	if (!CanSpawnClone())
	{
		CancelCloneCreation();
		
		UE_LOG(LogTemp, Display,
			TEXT("ABallPawn::SpawnClone: Unable to spawn Clone. Clone is colliding with player."));

		return;
	}

	SpawnCloneObject();
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

void ABallPawn::SpawnCloneObject()
{
	// Spawn SpawnCloneNiagaraSystem
	const UNiagaraComponent* SpawnCloneNiagaraSystem = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		this, SpawnCloneNiagaraSystemTemplate,
		CloneSpawnTransform.GetLocation());

	if (!IsValid(SpawnCloneNiagaraSystem))
	{
		UE_LOG(LogTemp, Warning, TEXT("ABallPawn::SpawnCloneObject: Failed to spawn SpawnCloneNiagaraSystem!"));
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

	if (!IsValid(SpawnCloneSound))
	{
		UE_LOG(LogTemp, Warning, TEXT("ABallPawn::SpawnCloneObject: SpawnCloneSound is invalid!"));
		
		return;
	}
	
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), SpawnCloneSound, GetActorLocation());
}

// ReSharper disable once CppMemberFunctionMayBeConst
void ABallPawn::SaveGame()
{
	if (SaveGameManagerPtr.IsValid())
	{
		SaveGameManagerPtr->SaveGame();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ABallPawn::SaveGame: SaveGameManagerPtr is invalid!"));
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void ABallPawn::LoadGame()
{
	if (SaveGameManagerPtr.IsValid())
	{
		SaveGameManagerPtr->LoadGame();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ABallPawn::LoadGame: SaveGameManagerPtr is invalid!"));
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

void ABallPawn::Die()
{
	if (!IsValid(DieSound))
	{
		UE_LOG(LogTemp, Warning, TEXT("ABallPawn::Die: DieSound is invalid!"));
		
		return;
	}
	
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), DieSound, GetActorLocation());
	
	// Spawn DeathNiagaraSystem
	const UNiagaraComponent* DeathNiagaraSystem = UNiagaraFunctionLibrary::SpawnSystemAtLocation(this,
		DeathNiagaraSystemTemplate, GetActorLocation());

	if (!IsValid(DeathNiagaraSystem))
	{
		UE_LOG(LogTemp, Warning, TEXT("ABallPawn::Die: Failed to spawn DeathNiagaraSystem!"));
	}

	// Destroy BallPawn if it called for clone
	if (!IsPlayerControlled())
	{
		Destroy();

		return;
	}

	// We can't die twice
	if (bDead)
	{
		return;
	}

	APlayerController* PlayerController = GetController<APlayerController>();

	if (!IsValid(PlayerController))
	{
		UE_LOG(LogTemp, Error, TEXT("ABallPawn::Die: PlayerController is invalid!"));

		return;
	}

	APlayerCameraManager* PlayerCameraManager = PlayerController->PlayerCameraManager;

	// Don't do anything until we will make sure PlayerCameraManager is valid
	if (!IsValid(PlayerCameraManager))
	{
		UE_LOG(LogTemp, Error, TEXT("ABallPawn::Die: PlayerCameraManager is invalid!"));

		return;
	}

	// Disable ANY player input
	DisableInput(PlayerController);
	PlayerController->DisableInput(PlayerController);

	// Instead of destroying Actor we will hide the mesh
	MeshComponent->SetHiddenInGame(true);

	// Disable physics to stop any movement
	MeshComponent->SetSimulatePhysics(false);

	// Ignore the Laser while the player is dead
	Tags.Add(IgnoreLaserTagName);

	// Fade the camera to black
	PlayerCameraManager->StartCameraFade(0, 1, DeathCameraFadeDuration, FLinearColor::Black,
		true, true);

	// Player can't create Clone while dead
	GetWorldTimerManager().ClearTimer(CreateCloneTimer);

	// Remember that player is dead
	bDead = true;

	// Start async loading to last save once the screen became fully black
	if (DeathCameraFadeDuration == 0)
	{
		LoadGame();
	}
	else
	{
		GetWorldTimerManager().SetTimer(LoadAfterDeathTimer, this, &ABallPawn::LoadGame,
			DeathCameraFadeDuration, false);
	}
}

void ABallPawn::Revive()
{
	// We can't revive if we're not dead
	if (!bDead)
	{
		return;
	}

	APlayerController* PlayerController = GetController<APlayerController>();

	if (!IsValid(PlayerController))
	{
		UE_LOG(LogTemp, Error, TEXT("ABallPawn::Revive: PlayerController is invalid!"));

		return;
	}

	APlayerCameraManager* PlayerCameraManager = PlayerController->PlayerCameraManager;

	// Don't do anything until we will make sure PlayerCameraManager is valid
	if (!IsValid(PlayerCameraManager))
	{
		UE_LOG(LogTemp, Error, TEXT("ABallPawn::Revive: PlayerCameraManager is invalid!"));

		return;
	}

	// Enable player input back
	EnableInput(PlayerController);
	PlayerController->EnableInput(PlayerController);

	// Show the mesh back
	MeshComponent->SetHiddenInGame(false);

	// Enable physics back
	MeshComponent->SetSimulatePhysics(true);

	// Stop ignoring the Laser
	Tags.Remove(IgnoreLaserTagName);

	// Fade the camera back from black
	PlayerCameraManager->StartCameraFade(1, 0, DeathCameraFadeDuration, FLinearColor::Black,
		true, true);

	// Remember that player isn't dead anymore
	bDead = false;
}