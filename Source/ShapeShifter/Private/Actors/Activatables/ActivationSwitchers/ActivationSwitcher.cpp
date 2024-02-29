// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/Activatables/ActivationSwitchers/ActivationSwitcher.h"

#include "SpiderNavGridBuilder.h"
#include "SpiderNavigation.h"
#include "Interfaces/Activatable.h"

void AActivationSwitcher::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ASpiderNavGridBuilder* GridBuilder = GetWorld()->SpawnActor<ASpiderNavGridBuilder>(SpawnParameters);
	GridBuilder->BuildGrid();

	ASpiderNavigation* Navigation = GetWorld()->SpawnActor<ASpiderNavigation>(SpawnParameters);

	for (const AActor* Actor : ActorsToSwitchActivation)
	{
		bool bFoundCompletePath;
		TArray<FVector> Path = Navigation->FindPath(GetActorLocation(), Actor->GetActorLocation(),
			bFoundCompletePath);

		FVector PreviousPoint = GetActorLocation();

		for (FVector& Point : Path)
		{
			DrawDebugLine(GetWorld(), PreviousPoint, Point, FColor::Blue);
			PreviousPoint = Point;
		}
	}

	GetComponents()

	GridBuilder->EmptyAll();
	GridBuilder->Destroy();

	Navigation->Destroy();
}

void AActivationSwitcher::SwitchActorsActivation() const
{
	// Iterate through ActorsToSwitchActivation
	for (AActor* It : ActorsToSwitchActivation)
	{
		// Cast It to ActorToSwitchActivation to use Activatable interface functions
		IActivatable* ActorToSwitchActivation = Cast<IActivatable>(It);

		// Check if ActorToSwitchActivation is valid
		if (ActorToSwitchActivation)
		{
			// Switch Active state
			ActorToSwitchActivation->SetActive(!ActorToSwitchActivation->IsActive());
		}
	}
}