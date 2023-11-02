#include "Actors/Activatables/MovingPlatform.h"

#include "Components/SplineComponent.h"

AMovingPlatform::AMovingPlatform()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = SceneComponent;
	
	MovementDirectionSplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("Movement Direction Spline"));
	MovementDirectionSplineComponent->SetupAttachment(RootComponent);
	
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MeshComponent->SetupAttachment(MovementDirectionSplineComponent);
}

void AMovingPlatform::BeginPlay()
{
	Super::BeginPlay();

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

void AMovingPlatform::ProcessMovementTimeline(const float Value)
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

	// Setting new location and rotation for platform
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

	// Start movement immediately if StartDelay is 0
	if (StartDelay == 0)
	{
		MovementTimeline.Play();
	}
	// Start movement with delay in another case
	else
	{
		GetWorldTimerManager().SetTimer(MoveTimer, [this]() {
			MovementTimeline.Play();
		}, StartDelay, false);
	}
}

void AMovingPlatform::Deactivate()
{
	bActive = false;
	MovementTimeline.Stop();

	GetWorldTimerManager().ClearTimer(MoveTimer);

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
		GetWorldTimerManager().SetTimer(MoveTimer, [this] {
			MovementTimeline.Reverse();
		}, EndDelay, false);
	}
}