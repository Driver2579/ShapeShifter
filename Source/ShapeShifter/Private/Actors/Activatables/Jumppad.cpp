// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/Activatables/Jumppad.h"

#include "Components/JumpPadTargetComponent.h"
#include "Components/BoxComponent.h"

AJumpPad::AJumpPad()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	BaseMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base Mesh"));
	BaseMeshComponent->SetupAttachment(RootComponent);

	PadMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Pad Mesh"));
	PadMeshComponent->SetupAttachment(BaseMeshComponent);

	JumpTriggerComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	JumpTriggerComponent->SetupAttachment(BaseMeshComponent);

	TargetLocationComponent = CreateDefaultSubobject<UJumpPadTargetComponent>(TEXT("Target Location"));
	TargetLocationComponent->SetupAttachment(RootComponent);
}

UStaticMeshComponent* AJumpPad::GetMesh() const
{
	return BaseMeshComponent;
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

	InitialVelocity = TargetLocationComponent->GetRelativeLocation().GetSafeNormal() * HorizontalVelocity;
	InitialVelocity.Z += VerticalVelocity;

	JumpTriggerComponent->OnComponentBeginOverlap.AddDynamic(this, &AJumpPad::OnJumpBeginTriggerOverlap);
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

void AJumpPad::OnJumpBeginTriggerOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsValid(OtherActor) || !OtherComp->IsSimulatingPhysics())
	{
		return;
	}
	
	// Jump the mesh if delay is 0
	if (JumpDelay == 0)
	{
		OtherComp->SetPhysicsLinearVelocity(InitialVelocity);
	}
	// Jump the mesh with delay
	else
	{
		GetWorldTimerManager().SetTimer(JumpTimer, [this, OtherComp]
		{
			OtherComp->SetPhysicsLinearVelocity(InitialVelocity);
		}, JumpDelay, false);
	}
}

void AJumpPad::OnJumpTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	GetWorldTimerManager().ClearTimer(JumpTimer);
}

//void AJumpPad::OnTransformUpdated(USceneComponent* UpdatedComponent, EUpdateTransformFlags UpdateTransformFlags, ETeleportType Teleport)
//{
	//const FVector NewDirection = TargetLocationComponent->GetRelativeLocation().GetSafeNormal();
	//float Angle = FMath::Acos(FVector::DotProduct(NewDirection, FVector::ForwardVector)) * (180.0f / PI);

	//const FRotator NewRotation = FRotator(0.0f, Angle, 0.0f);;
	//BaseMeshComponent->SetWorldRotation(NewRotation);
//}