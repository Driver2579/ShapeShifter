// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/HUDs/MainMenuHUD.h"

#include "Blueprint/UserWidget.h"

void AMainMenuHUD::BeginPlay()
{
    if (!MainMenuWidgetClass)
    {
        UE_LOG(LogTemp, Error, TEXT("AMainMenuHUD::BeginPlay: MainMenuWidgetClass is NONE!"));

        return;
    }

    MainMenuWidget = CreateWidget<UUserWidget>(GetWorld(), MainMenuWidgetClass);

    if (!MainMenuWidget)
    {
        UE_LOG(LogTemp, Error, TEXT("AMainMenuHUD::BeginPlay: MainMenuWidget is invalid!"));

        return;
    }

    MainMenuWidget->AddToViewport();
}