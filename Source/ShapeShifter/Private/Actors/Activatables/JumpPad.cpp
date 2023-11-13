// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/Activatables/JumpPad.h"

#include "Components/BoxComponent.h"

AJumpPad::AJumpPad()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	BaseMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base Mesh"));
	BaseMeshComponent->SetupAttachment(RootComponent);

	PadMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Pad Mesh"));
	PadMeshComponent->SetupAttachment(RootComponent);

	JumpTriggerComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Jump Trigger"));
	JumpTriggerComponent->SetupAttachment(RootComponent);

	TargetLocationComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Target Location"));
	TargetLocationComponent->SetupAttachment(RootComponent);
}

void AJumpPad::BeginPlay()
{
	Super::BeginPlay();

	// The player cannot jump onto a platform if the jump is below the platform
	if (TargetLocationComponent->GetRelativeLocation().Z > JumpHeight)
	{
		UE_LOG(LogTemp, Error, TEXT("AJumpPad:BeginPlay: JumpHeight is lower than TargetLocationComponent height"));
		
		return;
	}

	// Acceleration of gravity
	const float Gravity = 981.0f;

	// Velocity to achieve jumping heights
	const float VerticalVelocity = FMath::Sqrt(2 * Gravity * JumpHeight);

	// The time the object will spend in the air
	const float FlightTime = VerticalVelocity / Gravity + FMath::Sqrt(
		2 * (JumpHeight - TargetLocationComponent->GetRelativeLocation().Z) / Gravity);

	// Velocity to the TargetLocation
	const float HorizontalVelocity = FVector::VectorPlaneProject(TargetLocationComponent->GetRelativeLocation(),
		FVector(0, 0, 1)).Length() / FlightTime;

	ThrowVelocity = TargetLocationComponent->GetRelativeLocation().GetSafeNormal() * HorizontalVelocity;
	ThrowVelocity.Z += VerticalVelocity;

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
}

void AJumpPad::OnJumpTriggerBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsValid(OtherComp) || !OtherComp->IsSimulatingPhysics())
	{
		return;
	}

	// Throw the OtherComp immediately if delay is 0
	if (JumpDelay == 0)
	{
		OtherComp->SetAllPhysicsLinearVelocity(ThrowVelocity);
	}
	// Throw the OtherComp with delay
	else
	{
		GetWorldTimerManager().SetTimer(JumpTimer, [this, OtherComp]
		{
			OtherComp->SetAllPhysicsLinearVelocity(ThrowVelocity);
		}, JumpDelay, false);
	}
}

void AJumpPad::OnJumpTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	GetWorldTimerManager().ClearTimer(JumpTimer);
}