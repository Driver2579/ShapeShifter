// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Activatables/MovingPlatformWithRailAndEnds.h"

AMovingPlatformWithRailAndEnds::AMovingPlatformWithRailAndEnds()
{
	//RailStartScene = CreateDefaultSubobject<USceneComponent>(TEXT("Rail Start Scene"));
	//RailStartScene->SetupAttachment(RootComponent);

	//RailEndScene = CreateDefaultSubobject<USceneComponent>(TEXT("Rail End Scene"));
	//RailEndScene->SetupAttachment(RootComponent);
	//
	//RailStartStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Rail Start Mesh"));
	//RailStartStaticMesh->SetupAttachment(RootComponent);
	//
	//RailEndStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Rail End Mesh"));
	//RailEndStaticMesh->SetupAttachment(RootComponent);
}

void AMovingPlatformWithRailAndEnds::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	//RailStartStaticMesh->SetStaticMesh(RailStartAndEndStaticMesh);
	//RailEndStaticMesh->SetStaticMesh(RailStartAndEndStaticMesh);

	//RailStartStaticMesh->SetRelativeLocationAndRotation(
	//	Spline->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::Type::Local) -
	//	RailStartScene->GetRelativeLocation() + RailStartStaticMesh->GetRelativeLocation(),

	//	Spline->GetRotationAtSplinePoint(0, ESplineCoordinateSpace::Type::Local) -
	//	RailStartScene->GetRelativeRotation() + RailStartStaticMesh->GetRelativeRotation()
	//	);

	//RailStartScene->SetRelativeLocationAndRotation(
	//	Spline->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::Type::Local),
	//	Spline->GetRotationAtSplinePoint(0, ESplineCoordinateSpace::Type::Local)
	//	);
}
