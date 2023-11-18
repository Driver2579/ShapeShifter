// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/Activatables/JumpPad.h"

#include "Components/JumpPadTargetComponent.h"
#include "Components/BoxComponent.h"

AJumpPad::AJumpPad()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	BaseMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base Mesh"));
	BaseMeshComponent->SetupAttachment(RootComponent);

	JumpTriggerComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Jump Trigger"));
	JumpTriggerComponent->SetupAttachment(BaseMeshComponent);

	RotationAxisComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Rotation Axis"));
	RotationAxisComponent->SetupAttachment(BaseMeshComponent);

	PadMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Pad Mesh"));
	PadMeshComponent->SetupAttachment(RotationAxisComponent);

	TargetLocationComponent = CreateDefaultSubobject<UJumpPadTargetComponent>(TEXT("Target Location"));
	TargetLocationComponent->SetupAttachment(RootComponent);
}

void AJumpPad::BeginPlay()
{
	Super::BeginPlay();

	// The player cannot jump onto a platform if the JumpHeight is below the platform
	if (TargetLocationComponent->GetRelativeLocation().Z > JumpHeight)
	{
		UE_LOG(LogTemp, Error, TEXT("AJumpPad:BeginPlay: JumpHeight is lower than TargetLocationComponent height"));

		return;
	}

	// There should be a curve showing the throw animation
	if (!IsValid(AnimationCurve))
	{
		UE_LOG(LogTemp, Error, TEXT("AJumpPad:BeginPlay: AnimationCurve is invalid!"));

		return;
	}

	// JumpPad Initialize
	InitializeRotation();
	InitializeAnimationTimeline();
	InitializeThrowVelocity();

	JumpTriggerComponent->OnComponentBeginOverlap.AddDynamic(this, &AJumpPad::OnJumpTriggerBeginOverlap);
	JumpTriggerComponent->OnComponentEndOverlap.AddDynamic(this, &AJumpPad::OnJumpTriggerEndOverlap);
}

void AJumpPad::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorldTimerManager().ClearTimer(JumpTimer);
}

void AJumpPad::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Update animation frame
	if (AnimationTimeline.IsPlaying())
	{
		AnimationTimeline.TickTimeline(DeltaTime);
	}
} 

void AJumpPad::InitializeRotation()
{
	// Save the initial axis rotation
	StartRotation = RotationAxisComponent->GetRelativeRotation();

	// Save the final axis rotation
	TargetRotation = RotationAxisComponent->GetRelativeRotation() + RotationOffset;
}

void AJumpPad::InitializeAnimationTimeline()
{
	// Bind event to handle value changes on the timeline
	FOnTimelineFloatStatic ProgressAnimation;
	ProgressAnimation.BindUObject(this, &AJumpPad::ProgressAnimateTimeline);
	AnimationTimeline.AddInterpFloat(AnimationCurve, ProgressAnimation);

	// Bind event to handle the end of the timeline
	FOnTimelineEventStatic OnAnimationFinished;
	OnAnimationFinished.BindUObject(this, &AJumpPad::OnEndAnimation);
	AnimationTimeline.SetTimelineFinishedFunc(OnAnimationFinished);

	// Set the timeline length mode based on the last key point of the curve
	AnimationTimeline.SetTimelineLengthMode(TL_LastKeyFrame);
}

void AJumpPad::InitializeThrowVelocity()
{
	const FVector& TargetLocation = TargetLocationComponent->GetRelativeLocation();

	// Acceleration of gravity
	const float Gravity = 981.0f;

	// Velocity to achieve jumping heights
	const float VerticalVelocity = FMath::Sqrt(2 * Gravity * JumpHeight);

	// The time the object will spend in the air
	const float FlightTime = VerticalVelocity / Gravity + FMath::Sqrt(2 * (JumpHeight - TargetLocation.Z) / Gravity);

	// Path to TargetLocationComponent in XY plane
	FVector HorizontalPath = TargetLocation;
	HorizontalPath.Z = 0;

	// Velocity to the TargetLocationComponent
	const float HorizontalVelocity = HorizontalPath.Length() / FlightTime;

	// Set the throwing force
	ThrowVelocity = HorizontalPath.GetSafeNormal() * HorizontalVelocity;
	ThrowVelocity.Z += VerticalVelocity;
}

UStaticMeshComponent* AJumpPad::GetMesh() const
{
	return BaseMeshComponent;
}

void AJumpPad::OnJumpTriggerBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsValid(OtherComp) && OtherComp->IsSimulatingPhysics() && !AnimationTimeline.IsPlaying())
	{
		ThrowObject(OtherComp);
	}
}

void AJumpPad::OnJumpTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	GetWorldTimerManager().ClearTimer(JumpTimer);
}

void AJumpPad::ProgressAnimateTimeline(const float Value) const
{
	const FRotator& NewRotation = FMath::Lerp(StartRotation, TargetRotation, Value);
	RotationAxisComponent->SetRelativeRotation(NewRotation);
}

void AJumpPad::OnEndAnimation()
{
	TArray<UPrimitiveComponent*> OverlappingComponents;
	JumpTriggerComponent->GetOverlappingComponents(OverlappingComponents);

	// Throw all objects that are on the jumppad
	for (auto It : OverlappingComponents)
	{
		ThrowObject(It);
	}
}

void AJumpPad::ThrowObject(UPrimitiveComponent* Object)
{
	// Throw the OtherComp immediately if delay is 0
	if (JumpDelay == 0)
	{
		Object->SetAllPhysicsLinearVelocity(ThrowVelocity);
		AnimationTimeline.PlayFromStart();
	}
	// Throw the OtherComp with delay
	else
	{
		GetWorldTimerManager().SetTimer(JumpTimer, [this, Object]
			{
				Object->SetAllPhysicsLinearVelocity(ThrowVelocity);
				AnimationTimeline.PlayFromStart();
			}, JumpDelay, false);
	}
}