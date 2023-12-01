// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraActor.h"
#include "MainMenuCameraActor.generated.h"

UCLASS()
class SHAPESHIFTER_API AMainMenuCameraActor : public ACameraActor
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;
};