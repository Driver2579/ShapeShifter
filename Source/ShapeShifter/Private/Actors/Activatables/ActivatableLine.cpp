// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/Activatables/ActivatableLine.h"

#include "Components/SplineMeshComponent.h"
#include "Components/SplineMeshesComponent.h"

AActivatableLine::AActivatableLine()
{
	PrimaryActorTick.bCanEverTick = false;

	SplineMeshesComponent = CreateDefaultSubobject<USplineMeshesComponent>(TEXT("Spline Meshes"));
	SplineMeshesComponent->SetupAttachment(RootComponent);
}

void AActivatableLine::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	SetActive(bActive);
}

void AActivatableLine::BeginPlay()
{
	Super::BeginPlay();

	SetActive(bActive);
}

bool AActivatableLine::IsActive() const
{
	return bActive;
}

void AActivatableLine::Activate()
{
	SetMaterialsColor(MaterialsActiveColor);

	bActive = true;
}

void AActivatableLine::Deactivate()
{
	SetMaterialsColor(MaterialsInactiveColor);
	
	bActive = false;
}

void AActivatableLine::SetMaterialsColor(const FLinearColor& Color) const
{
	TInlineComponentArray<USplineMeshComponent*> SplineMeshComponents;
	GetComponents(SplineMeshComponents);

	for (USplineMeshComponent* SplineMeshComponent : SplineMeshComponents)
	{
		TArray<UMaterialInterface*> Materials = SplineMeshComponent->GetMaterials();

		for (int i = 0; i < Materials.Num(); ++i)
		{
			SplineMeshComponent->CreateDynamicMaterialInstance(i)->SetVectorParameterValue(MaterialParameterColorName,
				Color);
		}
	}
}
