// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/Activatables/MovingPlatform.h"
#include "Components/SplineComponent.h"

DEFINE_LOG_CATEGORY_STATIC(MovingPlatform, All, All);

AMovingPlatform::AMovingPlatform()
{
	PrimaryActorTick.bCanEverTick = true;
	
	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("Spline Component"));
	RootComponent = SplineComponent;
	
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	MeshComponent->SetupAttachment(RootComponent);
}

void AMovingPlatform::BeginPlay()
{
	Super::BeginPlay();

	if (!MovementCurve)
	{
		UE_LOG(MovingPlatform, Error, TEXT("%s: Movement Curve not found"), *GetName());

		return;
	}

	// Duplicate curve for editing and using
	MovementCurve = DuplicateObject(MovementCurve, nullptr);

	// Changing Animation Time
	for (FRichCurveKey& Key : MovementCurve->FloatCurve.Keys)
	{
		Key.Time *= MoveTime;
	}

	// Calibrate the spline position of mesh
	ProcessMovementTimeline(MovementCurve->FloatCurve.Keys[0].Value);

	// Calibrate the world position of mesh 
	const FVector SplineLocation = SplineComponent->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World);
	MeshComponent->SetWorldLocation(SplineLocation);

	FOnTimelineFloat ProgressFunction;
	ProgressFunction.BindUFunction(this, TEXT("ProcessMovementTimeline"));
	MovementTimeline.AddInterpFloat(MovementCurve, ProgressFunction);

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

	if(MovementTimeline.IsPlaying())
	{
		MovementTimeline.TickTimeline(DeltaTime);	
	}
}

void AMovingPlatform::ProcessMovementTimeline(float Value)
{
	const float Distance = Value * SplineComponent->GetSplineLength();

	const FVector CurrentSplineLocation = SplineComponent->GetLocationAtDistanceAlongSpline(Distance,
		ESplineCoordinateSpace::World);

	if (bRotate)
	{
		FRotator CurrentSplineRotation = SplineComponent->GetRotationAtDistanceAlongSpline(Distance,
			ESplineCoordinateSpace::World);
		CurrentSplineRotation.Pitch = 0.f;

		MeshComponent->SetWorldLocationAndRotation(CurrentSplineLocation, CurrentSplineRotation);
	}
	else
	{
		MeshComponent->SetWorldLocation(CurrentSplineLocation);
	}
}


void AMovingPlatform::Activate()
{
	bIsActive = true;

	if (MoveTime == 0)
	{
		UE_LOG(MovingPlatform, Warning, TEXT("%s: Move Time is 0"), *GetName());

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

	if (!bLoop)
	{
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
}

bool AMovingPlatform::IsActive() const
{
	return bIsActive;
}