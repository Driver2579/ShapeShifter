#include "Actors/Activatables/MovingPlatform.h"

#include "Components/SplineComponent.h"

AMovingPlatform::AMovingPlatform()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent =  CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = SceneComponent;
	
	MovementDirectionSplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("Movment Direction Spline"));
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

	if (!IsValid(MovementCurve))
	{
		UE_LOG(LogTemp, Error, TEXT("AMovingPlatform:BeginPlay: MovementCurve is invalid!"));

		return;
	}
	
	// Change animation time by MoveTime
	for (FRichCurveKey& Key : MovementCurve->FloatCurve.Keys)
	{
		Key.Time *= MoveTime;
	}

	// Calibrate the spline location of mesh
	ProcessMovementTimeline(MovementCurve->FloatCurve.Keys[0].Value);

	// Calibrate the world location of mesh 
	const FVector SplineLocation = MovementDirectionSplineComponent->GetLocationAtSplinePoint(0,
		ESplineCoordinateSpace::World);
	
	MeshComponent->SetWorldLocation(SplineLocation);

	//Create a delegate to handle value changes on the timeline
	FOnTimelineFloat ProgressFunction;
	ProgressFunction.BindUFunction(this, TEXT("ProcessMovementTimeline"));
	
	// Add motion curve interpolation to the timeline using the given delegate
	MovementTimeline.AddInterpFloat(MovementCurve, ProgressFunction);

	// Set the timeline length mode based on the last key point of the curve and set the timeline to cycle
	MovementTimeline.SetTimelineLengthMode(TL_LastKeyFrame);
	MovementTimeline.SetLooping(bLoop);
}

void AMovingPlatform::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorldTimerManager().ClearTimer(MoveTimer);
}

void AMovingPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Update platform position
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

	if (bRotate)
	{
		// Calculation of rotation in space
		FRotator CurrentSplineRotation = MovementDirectionSplineComponent->GetRotationAtDistanceAlongSpline(Distance,
			ESplineCoordinateSpace::World);

		CurrentSplineRotation.Pitch = 0.f;

		// Setting a new location and rotation
		MeshComponent->SetWorldLocationAndRotation(CurrentSplineLocation, CurrentSplineRotation);
	}
	else
	{
		// Setting a new position
		MeshComponent->SetWorldLocation(CurrentSplineLocation);
	}
}


void AMovingPlatform::Activate()
{
	bIsActive = true;

	if (MoveTime == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("AMovingPlatform::Activate: Platform can't be moved because MoveTime is 0!"));
		
		return;
	}

	GetWorldTimerManager().ClearTimer(MoveTimer);

	// Start with or without delay
	if (StartDelay == 0)
	{
		MovementTimeline.Play();
	}
	else
	{
		GetWorldTimerManager().SetTimer(MoveTimer, [this]() {
			MovementTimeline.Play();
		}, StartDelay, false);
	}
}

void AMovingPlatform::Deactivate()
{
	bIsActive = false;
	MovementTimeline.Stop();

	GetWorldTimerManager().ClearTimer(MoveTimer);

	// Infinite movement when activated and stop in place when deactivated
	if (bLoop) return;
	
	// Reverse with or without delay
	if (EndDelay == 0)
	{
		MovementTimeline.Reverse();
	}
	else
	{
		GetWorldTimerManager().SetTimer(MoveTimer, [this]{
			MovementTimeline.Reverse();
		}, EndDelay, false);
	}
}

bool AMovingPlatform::IsActive() const
{
	return bIsActive;
}