// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/HUDs/MainMenuHUD.h"

#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

void AMainMenuHUD::BeginPlay()
{
    Super::BeginPlay();

    if (!IsValid(MainMenuWidgetClass))
    {
        UE_LOG(LogTemp, Error, TEXT("AMainMenuHUD::BeginPlay: MainMenuWidgetClass is invalid!"));

        return;
    }

    MainMenuWidget = CreateWidget<UUserWidget>(GetWorld(), MainMenuWidgetClass);

    if (!MainMenuWidget.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("AMainMenuHUD::BeginPlay: Failed to create MainMenuWidget!"));

        return;
    }

    MainMenuWidget->AddToViewport();

    // Get PlayerController to change controls
    APlayerController* Controller = GetOwningPlayerController();

    if (!IsValid(Controller))
    {
        UE_LOG(LogTemp, Error, TEXT("AMainMenuHUD::BeginPlay: Controller is invalid!"));

        return;
    }

    Controller->bShowMouseCursor = true;
    Controller->SetInputMode(FInputModeUIOnly());
}