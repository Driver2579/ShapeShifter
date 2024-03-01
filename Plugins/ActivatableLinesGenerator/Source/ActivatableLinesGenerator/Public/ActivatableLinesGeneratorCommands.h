// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "ActivatableLinesGeneratorStyle.h"

class FActivatableLinesGeneratorCommands : public TCommands<FActivatableLinesGeneratorCommands>
{
public:

	FActivatableLinesGeneratorCommands()
		: TCommands<FActivatableLinesGeneratorCommands>(TEXT("ActivatableLinesGenerator"), NSLOCTEXT("Contexts", "ActivatableLinesGenerator", "ActivatableLinesGenerator Plugin"), NAME_None, FActivatableLinesGeneratorStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > PluginAction;
};
