// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/Activatables/ActivationSwitchers/ActivationSwitcher.h"

#include "Interfaces/Activatable.h"

void AActivationSwitcher::GetActorsToSwitchActivation(TArray<AActor*>& OutActorsToSwitchActivation) const
{
	OutActorsToSwitchActivation = ActorsToSwitchActivation;
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
