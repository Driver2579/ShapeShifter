// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/Activatables/Door.h"

#include "Components/AudioComponent.h"

ADoor::ADoor()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	BaseMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("Base Mesh");
	BaseMeshComponent->SetupAttachment(RootComponent);

	LeftDoorMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("Left Door Mesh");
	LeftDoorMeshComponent->SetupAttachment(RootComponent);

	RightDoorMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("Right Door Mesh");
	RightDoorMeshComponent->SetupAttachment(RootComponent);

	OpenSoundComponent = CreateDefaultSubobject<UAudioComponent>("Open Sound");
	OpenSoundComponent->SetupAttachment(RootComponent);
	
	CloseSoundComponent = CreateDefaultSubobject<UAudioComponent>("Close Sound");
	CloseSoundComponent->SetupAttachment(RootComponent);
}

void ADoor::BeginPlay()
{
	Super::BeginPlay();

	// Set open and closed left door states
	StartLeftDoorLocation = LeftDoorMeshComponent->GetRelativeLocation();
	EndLeftDoorLocation = StartLeftDoorLocation - FVector(MoveOffset, 0, 0);

	// Set open and closed right door states
	StartRightDoorLocation = RightDoorMeshComponent->GetRelativeLocation();
	EndRightDoorLocation = StartRightDoorLocation - FVector(-MoveOffset, 0, 0);
}

void ADoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const FVector& TargetLeftDoorLocation = bActive ? EndLeftDoorLocation : StartLeftDoorLocation;
	const FVector& NewLeftLocation = FMath::VInterpConstantTo(LeftDoorMeshComponent->GetRelativeLocation(),
		TargetLeftDoorLocation, DeltaTime, MoveSpeed);

	const FVector& TargetRightDoorLocation = bActive ? EndRightDoorLocation : StartRightDoorLocation;
	const FVector& NewRightLocation = FMath::VInterpConstantTo(RightDoorMeshComponent->GetRelativeLocation(),
		TargetRightDoorLocation, DeltaTime, MoveSpeed);

	// Stop sound if the door does not move.
	if (TargetLeftDoorLocation == NewLeftLocation)
	{
		OpenSoundComponent->Stop();
		CloseSoundComponent->Stop();
	}
	
	LeftDoorMeshComponent->SetRelativeLocation(NewLeftLocation);
	RightDoorMeshComponent->SetRelativeLocation(NewRightLocation);
}

bool ADoor::IsActive() const
{
	return bActive;
}

void ADoor::Activate()
{
	bActive = true;

	CloseSoundComponent->Stop();
	OpenSoundComponent->Play();
}

void ADoor::Deactivate()
{
	bActive = false;

	OpenSoundComponent->Stop();
	CloseSoundComponent->Play();
}