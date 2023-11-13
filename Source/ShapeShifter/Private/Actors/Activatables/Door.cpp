// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/Activatables/Door.h"

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
}

void ADoor::BeginPlay()
{
	Super::BeginPlay();

	// Set open and closed states
	StartLeftDoorLocation = LeftDoorMeshComponent->GetRelativeLocation();
	EndLeftDoorLocation = StartLeftDoorLocation - FVector(MoveOffset, 0, 0);
	StartRightDoorLocation = RightDoorMeshComponent->GetRelativeLocation();
	EndRightDoorLocation = StartRightDoorLocation - FVector(-MoveOffset, 0, 0);
}

void ADoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector NewLeftLocation = FMath::VInterpConstantTo(LeftDoorMeshComponent->GetRelativeLocation(),
		IsActive() ? EndLeftDoorLocation : StartLeftDoorLocation, DeltaTime, MoveSpeed);

	FVector NewRightLocation = FMath::VInterpConstantTo(RightDoorMeshComponent->GetRelativeLocation(),
		IsActive() ? EndRightDoorLocation : StartRightDoorLocation, DeltaTime, MoveSpeed);

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
}

void ADoor::Deactivate()
{
	bActive = false;
}