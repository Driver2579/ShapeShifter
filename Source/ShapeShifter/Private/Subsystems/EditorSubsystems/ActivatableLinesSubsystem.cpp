// Fill out your copyright notice in the Description page of Project Settings.

#include "Subsystems/EditorSubsystems/ActivatableLinesSubsystem.h"

#include "SpiderNavGridBuilder.h"
#include "SpiderNavigation.h"
#include "Actors/Activatables/ActivationSwitchers/ActivationSwitcher.h"

void UActivatableLinesSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	FWorldDelegates::OnPostWorldInitialization.AddUObject(this, &UActivatableLinesSubsystem::OnWorldInitialized);
	FEditorDelegates::OnMapOpened.AddUObject(this, &UActivatableLinesSubsystem::OnMapOpened);
}

void UActivatableLinesSubsystem::Deinitialize()
{
	Super::Deinitialize();

	FEditorDelegates::OnMapOpened.RemoveAll(this);
	FWorldDelegates::OnPostWorldInitialization.RemoveAll(this);
}

void UActivatableLinesSubsystem::OnWorldInitialized(UWorld* World, const UWorld::InitializationValues IVS)
{
	LastOpenedWorld = World;
}

void UActivatableLinesSubsystem::OnMapOpened(const FString& Filename, bool bAsTemplate) const
{
	if (!bAsTemplate)
	{
		RebuildActivatableLines(LastOpenedWorld.Get());
	}
}

void UActivatableLinesSubsystem::RebuildActivatableLines(const UWorld* World) const
{
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ASpiderNavGridBuilder* NavGridBuilder = LastOpenedWorld->SpawnActor<ASpiderNavGridBuilder>(SpawnParameters);

	NavGridBuilder->BuildGrid();
	NavGridBuilder->SaveGrid();

	ASpiderNavigation* Navigation = LastOpenedWorld->SpawnActor<ASpiderNavigation>(SpawnParameters);
	Navigation->LoadGrid();

	TArray<AActor*> ActivationSwitchers;
	UGameplayStatics::GetAllActorsOfClass(World, AActivationSwitcher::StaticClass(),
		ActivationSwitchers);

	if (ActivationSwitchers.IsEmpty())
	{
		return;
	}

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
				DrawDebugLine(World, PreviousPoint, Point, FColor::Blue, true);
				PreviousPoint = Point;
			}
		}
	}

	NavGridBuilder->EmptyAll();
	NavGridBuilder->Destroy();

	Navigation->Destroy();
}