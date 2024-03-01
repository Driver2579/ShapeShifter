// Copyright Epic Games, Inc. All Rights Reserved.

#include "ActivatableLinesGenerator.h"
#include "ActivatableLinesGeneratorStyle.h"
#include "ActivatableLinesGeneratorCommands.h"
#include "Misc/MessageDialog.h"
#include "ToolMenus.h"
#include "SpiderNavGridBuilder.h"
#include "SpiderNavigation.h"
#include "Actors/Activatables/ActivationSwitchers/ActivationSwitcher.h"

#define LOCTEXT_NAMESPACE "FActivatableLinesGeneratorModule"

void FActivatableLinesGeneratorModule::StartupModule()
{
	/**
	 * This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin
	 * file per-module.
	 */

	FActivatableLinesGeneratorStyle::Initialize();
	FActivatableLinesGeneratorStyle::ReloadTextures();

	FActivatableLinesGeneratorCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(FActivatableLinesGeneratorCommands::Get().PluginAction,
		FExecuteAction::CreateRaw(this,
			&FActivatableLinesGeneratorModule::PluginButtonClicked), FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this,
		&FActivatableLinesGeneratorModule::RegisterMenus));

	FWorldDelegates::OnPostWorldInitialization.AddRaw(this,
		&FActivatableLinesGeneratorModule::OnWorldInitialized);
}

void FActivatableLinesGeneratorModule::ShutdownModule()
{
	/**
	 * This function may be called during shutdown to clean up your module. For modules that support dynamic reloading,
	 * we call this function before unloading the module.
	 */

	FWorldDelegates::OnPostWorldInitialization.RemoveAll(this);

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FActivatableLinesGeneratorStyle::Shutdown();

	FActivatableLinesGeneratorCommands::Unregister();
}

void FActivatableLinesGeneratorModule::PluginButtonClicked() const
{
	RebuildActivatableLines(LastOpenedEditorWorld.Get());
}

void FActivatableLinesGeneratorModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FActivatableLinesGeneratorCommands::Get().PluginAction, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("PluginTools");
			{
				FToolMenuEntry& Entry = Section.AddEntry(
					FToolMenuEntry::InitToolBarButton(FActivatableLinesGeneratorCommands::Get().PluginAction));

				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

void FActivatableLinesGeneratorModule::OnWorldInitialized(UWorld* World, const UWorld::InitializationValues IVS)
{
	if (!World->IsGameWorld())
	{
		LastOpenedEditorWorld = World;
	}
}

void FActivatableLinesGeneratorModule::RebuildActivatableLines(UWorld* World)
{
	if (!IsValid(World))
	{
		UE_LOG(LogTemp, Error, TEXT("FActivatableLinesGeneratorModule::RebuildActivatableLines: World is invalid!"));

		return;
	}

	// Get all ActivationSwitchers on the scene
	TArray<AActor*> ActivationSwitchers;
	UGameplayStatics::GetAllActorsOfClass(World, AActivationSwitcher::StaticClass(), ActivationSwitchers);

	if (ActivationSwitchers.IsEmpty())
	{
		UE_LOG(LogTemp, Warning,
			TEXT("FActivatableLinesGeneratorModule::RebuildActivatableLines: No ActivationSwitchers found on the "
				"scene."));

		return;
	}

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// Spawn NavGridBuilder to build navigation grid
	ASpiderNavGridBuilder* NavGridBuilder = World->SpawnActor<ASpiderNavGridBuilder>(SpawnParameters);

	// Build navigation grid
	NavGridBuilder->BuildGrid();

	/**
	 * Save navigation grid to the file. We don't really need it but this is the only way in SpiderNavigation plugin to
	 * get this grid in the ASpiderNavigation class
	 */
	NavGridBuilder->SaveGrid();

	// Spawn Navigation and load navigation grid to be able to find the path
	ASpiderNavigation* Navigation = World->SpawnActor<ASpiderNavigation>(SpawnParameters);
	Navigation->LoadGrid();

	// Iterate through all ActivationSwitchers
	for (AActor* ActivationSwitcherActor : ActivationSwitchers)
	{
		const AActivationSwitcher* ActivationSwitcher = CastChecked<AActivationSwitcher>(ActivationSwitcherActor);

		TArray<AActor*> ActorsToSwitchActivation;
		ActivationSwitcher->GetActorsToSwitchActivation(ActorsToSwitchActivation);

		// Find path from ActivationSwitcher to its all ActorsToSwitchActivation
		for (const AActor* ActorToSwitchActivation : ActorsToSwitchActivation)
		{
			const FVector StartPoint = ActivationSwitcher->GetActorLocation();

			bool bFoundCompletePath;
			TArray<FVector> Path = Navigation->FindPath(StartPoint, ActorToSwitchActivation->GetActorLocation(),
				bFoundCompletePath);

			FVector PreviousPoint = StartPoint;

			for (FVector& CurrentPoint : Path)
			{
				// TODO: replace it with adding a line texture to the scene
				DrawDebugLine(World, PreviousPoint, CurrentPoint, FColor::Blue, true);

				PreviousPoint = CurrentPoint;
			}
		}
	}

	// Destroy navigation grid and NavGridBuilder as we don't need them anymore
	NavGridBuilder->EmptyAll();
	NavGridBuilder->Destroy();

	// Destroy Navigation as we don't need them anymore
	Navigation->Destroy();
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FActivatableLinesGeneratorModule, ActivatableLinesGenerator)