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

	// Set open and closed states
	StartLeftDoorLocation = LeftDoorMeshComponent->GetRelativeLocation();
	EndLeftDoorLocation = StartLeftDoorLocation - FVector(MoveOffset, 0, 0);
	StartRightDoorLocation = RightDoorMeshComponent->GetRelativeLocation();
	EndRightDoorLocation = StartRightDoorLocation - FVector(-MoveOffset, 0, 0);
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

void ADoor::BeginPlay()
{
	Super::BeginPlay();
}

void ADoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Door movement
	FVector NewLeftLocation;
	FVector NewRightLocation;

	if (IsActive())
	{
		NewLeftLocation = FMath::VInterpConstantTo(LeftDoorMeshComponent->GetRelativeLocation(),
			EndLeftDoorLocation, DeltaTime, MoveSpeed);
		
		NewRightLocation = FMath::VInterpConstantTo(RightDoorMeshComponent->GetRelativeLocation(),
			EndRightDoorLocation, DeltaTime, MoveSpeed);
	}
	else
	{
		NewLeftLocation = FMath::VInterpConstantTo(LeftDoorMeshComponent->GetRelativeLocation(),
			StartLeftDoorLocation, DeltaTime, MoveSpeed);

		NewRightLocation = FMath::VInterpConstantTo(RightDoorMeshComponent->GetRelativeLocation(),
			StartRightDoorLocation, DeltaTime, MoveSpeed);
	}

	LeftDoorMeshComponent->SetRelativeLocation(NewLeftLocation);
	RightDoorMeshComponent->SetRelativeLocation(NewRightLocation);
}