// Copyright Epic Games, Inc. All Rights Reserved.

#include "ActivatableLinesGeneratorStyle.h"
#include "ActivatableLinesGenerator.h"
#include "Framework/Application/SlateApplication.h"
#include "Styling/SlateStyleRegistry.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleMacros.h"

#define RootToContentDir Style->RootToContentDir

TSharedPtr<FSlateStyleSet> FActivatableLinesGeneratorStyle::StyleInstance = nullptr;

void FActivatableLinesGeneratorStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FActivatableLinesGeneratorStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FActivatableLinesGeneratorStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("ActivatableLinesGeneratorStyle"));
	return StyleSetName;
}


const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);

TSharedRef< FSlateStyleSet > FActivatableLinesGeneratorStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("ActivatableLinesGeneratorStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("ActivatableLinesGenerator")->GetBaseDir() / TEXT("Resources"));

	Style->Set("ActivatableLinesGenerator.PluginAction", new IMAGE_BRUSH_SVG(TEXT("PlaceholderButtonIcon"), Icon20x20));
	return Style;
}

void FActivatableLinesGeneratorStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FActivatableLinesGeneratorStyle::Get()
{
	return *StyleInstance;
}
