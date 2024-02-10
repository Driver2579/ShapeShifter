// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/HUDs/PlayerHUD.h"

#include "Blueprint/UserWidget.h"

void APlayerHUD::BeginPlay()
{
    Super::BeginPlay();

    if (!PauseWidgetClass)
    {
        UE_LOG(LogTemp, Error, TEXT("APlayerHUD::BeginPlay: PauseWidgetClass is invalid!"));

        return;
    }

    PauseWidget = CreateWidget<UUserWidget>(GetWorld(), PauseWidgetClass);

    if (!PauseWidget.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("APlayerHUD::BeginPlay: Failed to create PauseWidget!"));

        return;
    }

    PauseWidget->AddToViewport();

    // The pause menu should initially be hidden
    PauseWidget->SetVisibility(ESlateVisibility::Hidden);
}

void APlayerHUD::OpenPauseMenu()
{
    if (!PauseWidget.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("APlayerHUD::OpenPauseMenu: PauseWidget is invalid!"));
        
        return;
    }

    PauseWidget->SetVisibility(ESlateVisibility::Visible);
}

void APlayerHUD::ClosePauseMenu()
{
    if (!PauseWidget.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("APlayerHUD::OpenPauseMenu: PauseWidget is invalid!"));
        
        return;
    }
    
    PauseWidget->SetVisibility(ESlateVisibility::Hidden);
}