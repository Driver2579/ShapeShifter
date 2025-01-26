// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/Activatables/MovingPlatform.h"

#include "Common/Structs/SaveData/MovingPlatformSaveData.h"
#include "Components/SplineMeshesWithEdgesComponent.h"
#include "Objects/ShapeShifterSaveGame.h"
#include "Pawns/BallPawn.h"
#include "Components/AudioComponent.h"

AMovingPlatform::AMovingPlatform()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	MovementDirectionSplineComponent = CreateDefaultSubobject<USplineMeshesWithEdgesComponent>(
		TEXT("Movement Direction Spline"));

	MovementDirectionSplineComponent->SetupAttachment(RootComponent);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MeshComponent->SetupAttachment(MovementDirectionSplineComponent);

	CollisionAttachPointComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Collision Attach Point"));
	CollisionAttachPointComponent->SetupAttachment(MeshComponent);

	ActivateAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("Activate Audio"));
	ActivateAudioComponent->SetupAttachment(MeshComponent);

	DeactivateAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("Deactivate Audio"));
	DeactivateAudioComponent->SetupAttachment(MeshComponent);

	AmbientAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("Ambient Audio"));
	AmbientAudioComponent->SetupAttachment(MeshComponent);

	bRotate = false;
}

#if WITH_EDITOR
void AMovingPlatform::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	/**
	 * Remember the rotation of the platform before the bRotate was enabled. It's a raw pointer because we need to check
	 * if it was ever set.
	 */
	if (bRotate)
	{
		MeshComponentRelativeRotationBeforeRotateEnabled = MakeUnique<FRotator>(
			FRotator(MeshComponent->GetRelativeRotation()));
	}
	// Otherwise, if bRotate was disabled and the rotation was set before and restore it
	else if (MeshComponentRelativeRotationBeforeRotateEnabled)
	{
		MeshComponent->SetRelativeRotation(*MeshComponentRelativeRotationBeforeRotateEnabled);

		MeshComponentRelativeRotationBeforeRotateEnabled = nullptr;
	}

	// Set the platform to the starting point for the preview in the level editor
	if (MovementCurve && !MovementCurve->FloatCurve.Keys.IsEmpty())
	{
		if (MeshComponentInitialRelativeLocation)
		{
			MeshComponent->SetRelativeLocation(*MeshComponentInitialRelativeLocation);
		}

		if (MeshComponentInitialRelativeRotation)
		{
			MeshComponent->SetRelativeRotation(*MeshComponentInitialRelativeRotation);
		}

		MeshComponentInitialRelativeLocation = MakeUnique<FVector>(MeshComponent->GetRelativeLocation());
		MeshComponentInitialRelativeRotation = MakeUnique<FRotator>(MeshComponent->GetRelativeRotation());

		ProcessMovementTimeline(MovementCurve->FloatCurve.Keys[0].Value);
	}
	else
	{
		ResetPositionToDefault();
	}
}
#endif

void AMovingPlatform::BeginPlay()
{
	Super::BeginPlay();

	SetupCollisionComponents();

	if (!MovementCurve)
	{
		UE_LOG(LogTemp, Error, TEXT("AMovingPlatform:BeginPlay: MovementCurve is invalid!"));
		
		return;
	}

#if !NO_LOGGING
	// Check elements for validation
	for (const auto& Elem : DelaysMap)
	{
		if (Elem.Key < 0 || Elem.Key > MoveTime)
		{
			UE_LOG(LogTemp, Error, TEXT("AMovingPlatform::BeginPlay: DelaysMap key %f is invalid!"), Elem.Key);
		}

		if (Elem.Value < 0)
		{
			UE_LOG(LogTemp, Error, TEXT("AMovingPlatform::BeginPlay: DelaysMap value %f is invalid!"),
				Elem.Key);
		}
	}
#endif

#if WITH_EDITOR
	ResetPositionToDefault();
#endif

	MeshComponentInitialRelativeLocation = MakeUnique<FVector>(MeshComponent->GetRelativeLocation());
	MeshComponentInitialRelativeRotation = MakeUnique<FRotator>(MeshComponent->GetRelativeRotation());

	FOnTimelineEvent MovementTimelineEvent;
	MovementTimelineEvent.BindDynamic(this, &AMovingPlatform::OnMovementTimelineEvent);

	// Add an event to each key 
	for (const auto& Elem : DelaysMap)
	{
		MovementTimeline.AddEvent(Elem.Key, MovementTimelineEvent);
	}
	
	// Duplicate the curve for editing and using
	MovementCurve = DuplicateObject(MovementCurve, this);

	// Change animation time by MoveTime
	for (FRichCurveKey& Key : MovementCurve->FloatCurve.Keys)
	{
		Key.Time *= MoveTime;
	}

	// Move the platform to the starting point
	ProcessMovementTimeline(MovementCurve->FloatCurve.Keys[0].Value);

	// Add an event to handle value changes on the timeline
	FOnTimelineFloat ProgressFunction;
	ProgressFunction.BindDynamic(this, &AMovingPlatform::ProcessMovementTimeline);
	
	// Add motion curve interpolation to the timeline using the given delegate
	MovementTimeline.AddInterpFloat(MovementCurve, ProgressFunction);

	// Set the timeline length mode based on the last key point of the curve
	MovementTimeline.SetTimelineLengthMode(TL_LastKeyFrame);

	// Set the timeline to cycle
	MovementTimeline.SetLooping(bLoop);

	// Set to activated location if necessary
	SetActive(bActive);
}

void AMovingPlatform::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	MeshComponentInitialRelativeLocation = nullptr;
	MeshComponentInitialRelativeRotation = nullptr;

	GetWorldTimerManager().ClearTimer(MoveTimer);
}

void AMovingPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Update platform location
	if (MovementTimeline.IsPlaying())
	{
		MovementTimeline.TickTimeline(DeltaTime);	
	}
}

void AMovingPlatform::OnMovementTimelineEvent()
{
	bool bReversePlayback = MovementTimeline.IsReversing();
	
	MovementTimeline.Stop();

	// Current time
	const float Time = MovementTimeline.GetPlaybackPosition();

	TMap<float, float>::TConstIterator It = DelaysMap.CreateConstIterator();

	// Values from the first element
	float CurrentDelayAccuracy = FMath::Abs(Time - It.Key());
	float CurrentDelayDuration = It.Value();

	// Find the most similar to Time
	while (++It)
	{
		const float NewDelayAccuracy = FMath::Abs(Time - It.Key());

		// Reassign more profitable
		if (NewDelayAccuracy < CurrentDelayAccuracy)
		{
			CurrentDelayAccuracy = NewDelayAccuracy;
			CurrentDelayDuration = It.Value();
		}
	}

	// Return the platform to motion after a while
	GetWorldTimerManager().SetTimer(MoveTimer, [this, bReversePlayback]
	{
		if (!IsActive() && bLoop)
		{
			return;
		}
		
		if (bReversePlayback)
		{
			MovementTimeline.Reverse();
		}
		else
		{
			MovementTimeline.Play();
		}
	}, CurrentDelayDuration, false);
}

void AMovingPlatform::SetupCollisionComponents() const
{
	TArray<USceneComponent*> AttachedComponents;
	CollisionAttachPointComponent->GetChildrenComponents(true, AttachedComponents);

	for (USceneComponent* AttachedComponent : AttachedComponents)
	{
		UPrimitiveComponent* CollisionComponent = Cast<UPrimitiveComponent>(AttachedComponent);

		if (IsValid(CollisionComponent))
		{
			CollisionComponent->OnComponentBeginOverlap.AddDynamic(this,
				&AMovingPlatform::OnCollisionComponentsBeginOverlap);
		}
	}
}

void AMovingPlatform::ResetPositionToDefault()
{
	MeshComponentInitialRelativeLocation = nullptr;
	MeshComponentInitialRelativeRotation = nullptr;

	const AMovingPlatform* DefaultObject = GetClass()->GetDefaultObject<AMovingPlatform>();

	MeshComponent->SetRelativeLocation(DefaultObject->MeshComponent->GetRelativeLocation());
	MeshComponent->SetRelativeRotation(DefaultObject->MeshComponent->GetRelativeRotation());
}

void AMovingPlatform::OnCollisionComponentsBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ABallPawn* BallPawn = Cast<ABallPawn>(OtherActor);

	// Kill BallPawn if it's in MovingPlatform
	if (IsValid(BallPawn))
	{
		BallPawn->Die();
	}
}

void AMovingPlatform::OnSaveGame(UShapeShifterSaveGame* SaveGameObject)
{
	if (!IsValid(SaveGameObject))
	{
		UE_LOG(LogTemp, Error, TEXT("AMovingPlatform::OnSaveGame: SaveGameObject is invalid!"));

		return;
	}

	FMovingPlatformSaveData MovingPlatformSaveData;

	MovingPlatformSaveData.bActive = bActive;
	MovingPlatformSaveData.PlaybackPosition = MovementTimeline.GetPlaybackPosition();

	/**
	 * Save MovingPlatform unique name and MovingPlatformSaveData. Add() will also replace MovingPlatformSaveData value
	 * instead of adding a key duplicate if key with unique name was already saved before.
	 */
	SaveGameObject->MovingPlatformSaveData.Add(GetName(), MovingPlatformSaveData);
}

void AMovingPlatform::OnLoadGame(UShapeShifterSaveGame* SaveGameObject)
{
	if (!IsValid(SaveGameObject))
	{
		UE_LOG(LogTemp, Error, TEXT("AMovingPlatform::OnSaveGame: SaveGameObject is invalid!"));

		return;
	}

	// Get saved MovingPlatformSaveData by MovingPlatform unique name
	const FMovingPlatformSaveData* MovingPlatformSaveData = SaveGameObject->MovingPlatformSaveData.Find(GetName());

	if (MovingPlatformSaveData == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("AMovingPlatform::OnLoadGame: MovingPlatformSaveData is invalid for %s"),
			*GetName());

		return;
	}

	// Load saved PlaybackPosition by MovingPlatform unique name
	MovementTimeline.SetPlaybackPosition(MovingPlatformSaveData->PlaybackPosition, false, true);

	// Clear MoveTimer in case if it was set before loading
	GetWorldTimerManager().ClearTimer(MoveTimer);

	if (MovingPlatformSaveData->bActive)
	{
		MovementTimeline.Play();
	}
	else if (!bLoop)
	{
		MovementTimeline.Reverse();
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void AMovingPlatform::ProcessMovementTimeline(const float Value)
{
	// Location on the spline
	float Distance;

	if (MovementDirectionSplineComponent->IsClosedLoop())
	{
		Distance = Value * MovementDirectionSplineComponent->GetSplineLength();
	}
	// Exclude the edges of the spline if the spline isn't the closed loop
	else
	{
		Distance = DistanceBetweenSplineEdges + Value *
			(MovementDirectionSplineComponent->GetSplineLength() - DistanceBetweenSplineEdges * 2);
	}

	// Calculation of location in space
	const FVector CurrentSplineLocation = MovementDirectionSplineComponent->GetLocationAtDistanceAlongSpline(Distance,
		ESplineCoordinateSpace::World);

	// Set a new location for the platform
	MeshComponent->SetWorldLocation(CurrentSplineLocation + *MeshComponentInitialRelativeLocation);

	// The platform has shifted in the indicated direction
	if (!bRotate)
	{
		return;
	}

	// Get the rotation of the platform on the current spline's location
	FRotator CurrentSplineRotation = MovementDirectionSplineComponent->GetRotationAtDistanceAlongSpline(Distance,
		ESplineCoordinateSpace::Local);

	// Set the initial rotation of the platform for Pitch and Roll, we need to rotate only Yaw
	CurrentSplineRotation.Pitch = MeshComponentInitialRelativeRotation->Pitch;
	CurrentSplineRotation.Roll = MeshComponentInitialRelativeRotation->Roll;

	// Set new rotation for the platform
	MeshComponent->SetRelativeRotation(CurrentSplineRotation);
}

bool AMovingPlatform::IsActive() const
{
	return bActive;
}

void AMovingPlatform::Activate()
{
	bActive = true;

	GetWorldTimerManager().ClearTimer(MoveTimer);

	ActivateAudioComponent->Play();
	DeactivateAudioComponent->Stop();
	AmbientAudioComponent->Play();
	
	MovementTimeline.Play();
}

void AMovingPlatform::Deactivate()
{
	bActive = false;
	
	ActivateAudioComponent->Stop();
	DeactivateAudioComponent->Play();
	AmbientAudioComponent->Stop();

	GetWorldTimerManager().ClearTimer(MoveTimer);

	// If the platform is not looped, it should not move to the starting location
	if (bLoop)
	{
		MovementTimeline.Stop();
	}
	else
	{
		MovementTimeline.Reverse();
	}
}