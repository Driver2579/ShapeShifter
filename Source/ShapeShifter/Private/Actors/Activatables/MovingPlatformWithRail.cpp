// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Activatables/MovingPlatformWithRail.h"

#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"

AMovingPlatformWithRail::AMovingPlatformWithRail()
{
	RailStartScene = CreateDefaultSubobject<USceneComponent>(TEXT("Rail Start Scene"));
	RailStartScene->SetupAttachment(RootComponent);

	RailEndScene = CreateDefaultSubobject<USceneComponent>(TEXT("Rail End Scene"));
	RailEndScene->SetupAttachment(RootComponent);
	
	RailStartStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Rail Start Mesh"));
	RailStartStaticMesh->SetupAttachment(RootComponent);
	
	RailEndStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Rail End Mesh"));
	RailEndStaticMesh->SetupAttachment(RootComponent);
}


void AMovingPlatformWithRail::BeginPlay()
{
	Super::BeginPlay();

	ConstructRail();
}

#if WITH_EDITOR
void AMovingPlatformWithRail::OnConstruction(const FTransform& Transform)
{
	// TInlineComponentArray<USplineMeshComponent*> Components;
	// GetComponents<USplineMeshComponent>(Components);
	//
	// for (USplineMeshComponent* Component : Components)
	// {
	// 	Component->DestroyComponent();
	// }

	for (USplineMeshComponent* Rail : Rails)
	{
		Rail->DestroyComponent();
	}
	
	//Rails.RemoveAll();
	
	ConstructRail();
}
#endif

void AMovingPlatformWithRail::ConstructRail()
{
	USplineComponent* Spline = MovementDirectionSplineComponent;
	
	const int32 NumSegments = Spline->GetNumberOfSplinePoints() - 1;

	for (int32 i = 0; i < NumSegments; ++i)
	{
		if (USplineMeshComponent* SplineMeshComponent = NewObject<USplineMeshComponent>(this))
		{
			SplineMeshComponent->SetMobility(EComponentMobility::Stationary);

			SplineMeshComponent->SetStaticMesh(RailStaticMesh);

			SplineMeshComponent->SetStartAndEnd(
				Spline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Type::Local),
				Spline->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::Type::Local),
				Spline->GetLocationAtSplinePoint(i + 1, ESplineCoordinateSpace::Type::Local),
				Spline->GetTangentAtSplinePoint(i + 1, ESplineCoordinateSpace::Type::Local)
			);

			SplineMeshComponent->AttachToComponent(Spline, FAttachmentTransformRules::KeepRelativeTransform);
			SplineMeshComponent->RegisterComponent();

			Rails.Add(SplineMeshComponent);
		}
	}

	RailStartStaticMesh->SetStaticMesh(RailStartAndEndStaticMesh);
	RailEndStaticMesh->SetStaticMesh(RailStartAndEndStaticMesh);

	RailStartStaticMesh->SetRelativeLocationAndRotation(
		Spline->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::Type::Local) -
		RailStartScene->GetRelativeLocation() + RailStartStaticMesh->GetRelativeLocation(),

		Spline->GetRotationAtSplinePoint(0, ESplineCoordinateSpace::Type::Local) -
		RailStartScene->GetRelativeRotation() + RailStartStaticMesh->GetRelativeRotation()
		);

	RailStartScene->SetRelativeLocationAndRotation(
		Spline->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::Type::Local),
		Spline->GetRotationAtSplinePoint(0, ESplineCoordinateSpace::Type::Local)
		);

}