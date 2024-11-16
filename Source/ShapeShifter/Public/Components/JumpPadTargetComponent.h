// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "JumpPadTargetComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SHAPESHIFTER_API UJumpPadTargetComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	UJumpPadTargetComponent();

protected:
#if WITH_EDITOR
	virtual void OnUpdateTransform(EUpdateTransformFlags UpdateTransformFlags,
		ETeleportType Teleport = ETeleportType::None) override;
#endif
};