#include "Actors/Activatables/MovingPlatform.h"

#include "Common/Structs/SaveData/MovingPlatformSaveData.h"
#include "Components/SplineComponent.h"
#include "Objects/ShapeShifterSaveGame.h"
#include "Pawns/BallPawn.h"
#include "Components/AudioComponent.h"


AMovingPlatform::AMovingPlatform()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	
	MovementDirectionSplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("Movement Direction Spline"));
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
}

void AMovingPlatform::BeginPlay()
{
	Super::BeginPlay();

	SetupCollisionComponents();

	if (!IsValid(MovementCurve))
	{
		UE_LOG(LogTemp, Error, TEXT("AMovingPlatform:BeginPlay: MovementCurve is invalid!"));
		
		return;
	}

	// Duplicate curve for editing and using
	MovementCurve = DuplicateObject(MovementCurve, nullptr);

	// Change animation time by MoveTime
	for (FRichCurveKey& Key : MovementCurve->FloatCurve.Keys)
	{
		Key.Time *= MoveTime;
	}

	// Move platform to starting point
	ProcessMovementTimeline(MovementCurve->FloatCurve.Keys[0].Value);

	const FVector SplineLocation = MovementDirectionSplineComponent->GetLocationAtSplinePoint(0,
		ESplineCoordinateSpace::World);
	
	MeshComponent->SetWorldLocation(SplineLocation);

	// Add an event to handle value changes on the timeline
	FOnTimelineFloat ProgressFunction;
	ProgressFunction.BindUFunction(this, TEXT("ProcessMovementTimeline"));
	
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
		UE_LOG(LogTemp, Error, TEXT("AMovingPlatform::OnLoadGame: MovingPlatformSaveData is invalid for %s"), *GetName());

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

void AMovingPlatform::ProcessMovementTimeline(const float Value) const
{
	// Location on the spline
	const float Distance = Value * MovementDirectionSplineComponent->GetSplineLength();

	// Calculation of location in space
	const FVector CurrentSplineLocation = MovementDirectionSplineComponent->GetLocationAtDistanceAlongSpline(Distance,
		ESplineCoordinateSpace::World);

	// Set new location for platform
	MeshComponent->SetWorldLocation(CurrentSplineLocation);

	// The platform has shifted in the indicated direction
	if (!bRotate)
	{
		return;
	}

	// Calculation of rotation in space
	FRotator CurrentSplineRotation = MovementDirectionSplineComponent->GetRotationAtDistanceAlongSpline(Distance,
		ESplineCoordinateSpace::World);

	CurrentSplineRotation.Pitch = 0.f;

	// Set new rotation for platform
	MeshComponent->SetWorldRotation(CurrentSplineRotation);
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
	
	// Start movement immediately if StartDelay is 0
	if (StartDelay == 0)
	{
		MovementTimeline.Play();
	}
	// Start movement with delay in another case
	else
	{
		GetWorldTimerManager().SetTimer(MoveTimer, [this]()
		{
			MovementTimeline.Play();
		}, StartDelay, false);
	}
}

void AMovingPlatform::Deactivate()
{
	bActive = false;

	MovementTimeline.Stop();
 
	GetWorldTimerManager().ClearTimer(MoveTimer);
	
	ActivateAudioComponent->Stop();
	DeactivateAudioComponent->Play();
	AmbientAudioComponent->Stop();

	// If the platform is not looped it should not move to the starting location
	if (bLoop)
	{
		return;
	}

	// Start reversing movement immediately if EndDelay is 0
	if (EndDelay == 0)
	{
		MovementTimeline.Reverse();
	}
	// Start reversing with delay in another case
	else
	{
		GetWorldTimerManager().SetTimer(MoveTimer, [this]
		{
			MovementTimeline.Reverse();
		}, EndDelay, false);
	}
}