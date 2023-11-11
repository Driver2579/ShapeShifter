// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Activatables/Jumppad.h"

#include "Components/BoxComponent.h"

AJumppad::AJumppad()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	BaseMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base Mesh"));
	BaseMeshComponent->SetupAttachment(RootComponent);

	PadMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Pad Mesh"));
	PadMeshComponent->SetupAttachment(RootComponent);

	TriggerComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	TriggerComponent->SetupAttachment(RootComponent);

	TargetLocation = CreateDefaultSubobject<USceneComponent>(TEXT("Target Location"));
	TargetLocation->SetupAttachment(RootComponent);
}

void AJumppad::BeginPlay()
{
	Super::BeginPlay();

	// The player cannot jump onto a platform if the jump is below the platform.
	if (TargetLocation->GetRelativeLocation().Z > JumpHeight)
	{
		UE_LOG(LogTemp, Error, TEXT("AJumppad:BeginPlay: Jump lower than TargetLocation"));
		
		return;
	}

	// Acceleration of gravity
	const float Gravity = 981.0f;

	// Velocity to achieve jumping heights
	const float VerticalVelocity = FMath::Sqrt(2 * Gravity * JumpHeight);

	// The time the mouse will spend in the air
	const float FlightTime = VerticalVelocity / Gravity + FMath::Sqrt(
		2 * (JumpHeight - TargetLocation->GetRelativeLocation().Z) / Gravity);

	// Velocity to the TargetLocation
	const float HorizontalVelocity = FVector::VectorPlaneProject(TargetLocation->GetRelativeLocation(),
		FVector(0, 0, 1)).Length() / FlightTime;

	InitialVelocity = TargetLocation->GetRelativeLocation().GetSafeNormal() * HorizontalVelocity;
	InitialVelocity.Z += VerticalVelocity;

	TriggerComponent->OnComponentBeginOverlap.AddDynamic(this, &AJumppad::OnTriggerOverlapBegin);
	TriggerComponent->OnComponentEndOverlap.AddDynamic(this, &AJumppad::OnTriggerEndBegin);
}

void AJumppad::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorldTimerManager().ClearTimer(JumpTimer);
}

void AJumppad::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AJumppad::OnTriggerOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsValid(OtherActor))
	{
		return;
	}
	
	TArray<UStaticMeshComponent*> MeshComponents;
	OtherActor->GetComponents<UStaticMeshComponent>(MeshComponents);

	// Jump the mesh if delay is 0
	if (JumpDelay == 0)
	{
		for (UStaticMeshComponent* MeshComponent : MeshComponents)
		{
			MeshComponent->SetPhysicsLinearVelocity(InitialVelocity); 
		}
	}
	// Jump the mesh with delay
	else
	{
		GetWorldTimerManager().SetTimer(JumpTimer, [this, MeshComponents]
		{
			for (UStaticMeshComponent* MeshComponent : MeshComponents)
			{
				MeshComponent->SetPhysicsLinearVelocity(InitialVelocity); 
			}
		}, JumpDelay, false);
	}
}

void AJumppad::OnTriggerEndBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	GetWorldTimerManager().ClearTimer(JumpTimer);
}