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
        UE_LOG(LogTemp, Error, TEXT("AMainMenuHUD::BeginPlay: MainMenuWidget is invalid!"));

        return;
    }

    MainMenuWidget->AddToViewport();

    // Get PlayerController to change controls
    APlayerController* Controller = GetOwningPlayerController(); //UGameplayStatics::GetPlayerController(GetWorld(), 0);

    if (!IsValid(Controller))
    {
        UE_LOG(LogTemp, Error, TEXT("AMainMenuHUD::BeginPlay: Controller is invalid!"));

        return;
    }

    Controller->bShowMouseCursor = true;
    Controller->SetInputMode(FInputModeUIOnly());
}