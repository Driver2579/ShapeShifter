// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FToolBarBuilder;
class FMenuBuilder;

class FActivatableLinesGeneratorModule : public IModuleInterface
{
public:
	// IModuleInterface implementation
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	// This function will be bound to Command
	void PluginButtonClicked() const;
	
private:
	void RegisterMenus();

	TSharedPtr<FUICommandList> PluginCommands;

	TWeakObjectPtr<UWorld> LastOpenedEditorWorld;

	void OnWorldInitialized(UWorld* World, const UWorld::InitializationValues IVS);

	static void RebuildActivatableLines(UWorld* World);
};
