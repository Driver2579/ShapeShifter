// Fill out your copyright notice in the Description page of Project Settings.

#include "Subsystems/EditorSubsystems/ActivatableLinesSubsystem.h"

#include "SpiderNavGridBuilder.h"
#include "SpiderNavigation.h"
#include "Actors/Activatables/ActivationSwitchers/ActivationSwitcher.h"

#if WITH_EDITOR

void UActivatableLinesSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	FEditorDelegates::OnMapOpened.AddUObject(this, &UActivatableLinesSubsystem::OnMapOpened);
}

void UActivatableLinesSubsystem::Deinitialize()
{
	Super::Deinitialize();

	FEditorDelegates::OnMapOpened.RemoveAll(this);
}

void UActivatableLinesSubsystem::OnMapOpened(const FString& Filename, bool bAsTemplate) const
{
	if (bAsTemplate)
	{
		return;
	}

	TArray<AActor*> ActivationSwitchers;
	UGameplayStatics::GetAllActorsOfClass(this, AActivationSwitcher::StaticClass(),
		ActivationSwitchers);

	if (ActivationSwitchers.IsEmpty())
	{
		return;
	}

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ASpiderNavGridBuilder* GridBuilder = GetWorld()->SpawnActor<ASpiderNavGridBuilder>(SpawnParameters);
	GridBuilder->BuildGrid();

	ASpiderNavigation* Navigation = GetWorld()->SpawnActor<ASpiderNavigation>(SpawnParameters);

	for (AActor* ActivationSwitcherActor : ActivationSwitchers)
	{
		const AActivationSwitcher* ActivationSwitcher = CastChecked<AActivationSwitcher>(ActivationSwitcherActor);

		TArray<AActor*> ActorsToSwitchActivation;
		ActivationSwitcher->GetActorsToSwitchActivation(ActorsToSwitchActivation);

		for (const AActor* ActorToSwitchActivation : ActorsToSwitchActivation)
		{
			const FVector StartPoint = ActivationSwitcher->GetActorLocation();

			bool bFoundCompletePath;
			TArray<FVector> Path = Navigation->FindPath(StartPoint, ActorToSwitchActivation->GetActorLocation(),
				bFoundCompletePath);

			FVector PreviousPoint = StartPoint;

			for (FVector& Point : Path)
			{
				DrawDebugLine(GetWorld(), PreviousPoint, Point, FColor::Blue, true);
				PreviousPoint = Point;
			}
		}
	}

	GridBuilder->EmptyAll();
	GridBuilder->Destroy();

	Navigation->Destroy();
}

#endif