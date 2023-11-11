// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/Activatables/ActivationSwitchers/Lever.h"

#include "Components/BoxComponent.h"

ALever::ALever()
{
 	// Set this actor to call Tick() every frame
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	BaseMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base Mesh"));
	BaseMeshComponent->SetupAttachment(RootComponent);

	LeverMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Lever Mesh"));
	LeverMeshComponent->SetupAttachment(RootComponent);

	// Enable Hit Events
	LeverMeshComponent->SetNotifyRigidBodyCollision(true);

	ActivateZoneComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Activate Zone"));
	ActivateZoneComponent->SetupAttachment(LeverMeshComponent);

	DeactivateZoneComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Deactivate Zone"));
	DeactivateZoneComponent->SetupAttachment(LeverMeshComponent);
}

void ALever::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (bActive)
	{
		LeverMeshComponent->SetRelativeRotation(LeverMeshActiveRotation);
	}
	else
	{
		LeverMeshComponent->SetRelativeRotation(LeverMeshInactiveRotation);
	}
}

void ALever::BeginPlay()
{
	Super::BeginPlay();

	LeverMeshComponent->OnComponentHit.AddDynamic(this, &ALever::OnLeverMeshHit);
}

void ALever::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Return to safe performance if we don't need to rotate LeverMeshComponent
	if (!bRotateLeverMesh)
	{
		return;
	}

	// Get LeverMeshTargetRotation by bActive state
	const FRotator& LeverMeshTargetRotation = bActive ? LeverMeshActiveRotation : LeverMeshInactiveRotation;

	const FRotator& NewRotation = FMath::RInterpTo(LeverMeshComponent->GetRelativeRotation(),
		LeverMeshTargetRotation, DeltaTime, RotationSpeed);

	LeverMeshComponent->SetRelativeRotation(NewRotation);

	// We need this because the default Tolerance in Equals function is too small and it could fail sometimes
	const float Tolerance = 0.001;

	if (LeverMeshComponent->GetRelativeRotation().Equals(LeverMeshTargetRotation, Tolerance))
	{
		bRotateLeverMesh = false;
	}
}

void ALever::OnLeverMeshHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// We don't need to handle OtherComps without physics
	if (!IsValid(OtherComp) || !OtherComp->IsSimulatingPhysics())
	{
		return;
	}

	// Get ZVelocity of OtherComp
	const float ZVelocity = OtherComp->GetComponentVelocity().Z;

	// Activate the Lever if OtherComp hit the top of it
	if (ZVelocity > VelocityToSwitchActivation && OtherComp->IsOverlappingComponent(ActivateZoneComponent))
	{
		Activate();
	}
	// Deactivate the Lever if OtherComp hit the bottom of it
	else if (ZVelocity < -VelocityToSwitchActivation && OtherComp->IsOverlappingComponent(DeactivateZoneComponent))
	{
		Deactivate();
	}
}

bool ALever::IsActive() const
{
	return bActive;
}

void ALever::Activate()
{
	// We don't need to do anything if Lever is already active
	if (bActive)
	{
		return;
	}

	bActive = true;
	OnActiveSwitch();
}

void ALever::Deactivate()
{
	// We don't need to do anything if Lever is already inactive
	if (!bActive)
	{
		return;
	}

	bActive = false;
	OnActiveSwitch();
}

void ALever::OnActiveSwitch()
{
	// Turn on LeverMeshComponent rotation because activation was switched
	bRotateLeverMesh = true;

	SwitchActorsActivation();
}