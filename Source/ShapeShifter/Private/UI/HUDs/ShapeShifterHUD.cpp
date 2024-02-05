// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/HUDs/ShapeShifterHUD.h"

#include "Blueprint/UserWidget.h"

void AShapeShifterHUD::BeginPlay()
{
    Super::BeginPlay();

    if (!PauseWidgetClass)
    {
        UE_LOG(LogTemp, Error, TEXT("AShapeShifterHUD::BeginPlay: PauseWidgetClass is invalid!"));

        return;
    }

    PauseWidget = CreateWidget<UUserWidget>(GetWorld(), PauseWidgetClass);

    if (!PauseWidget.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("AShapeShifterHUD::BeginPlay: Failed to create PauseWidget!"));

        return;
    }

    PauseWidget->AddToViewport();

    // The pause menu should initially be hidden
    PauseWidget->SetVisibility(ESlateVisibility::Hidden);
}

void AShapeShifterHUD::OpenPauseMenu()
{
    PauseWidget->SetVisibility(ESlateVisibility::Visible);
}

void AShapeShifterHUD::ClosePauseMenu()
{
    PauseWidget->SetVisibility(ESlateVisibility::Hidden);
}