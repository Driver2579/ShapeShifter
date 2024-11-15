// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "JumpPadTargetComponent.generated.h"

class UBillboardComponent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SHAPESHIFTER_API UJumpPadTargetComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	UJumpPadTargetComponent();

protected:
	virtual void OnRegister() override;
	
#if WITH_EDITOR
	virtual void OnUpdateTransform(EUpdateTransformFlags UpdateTransformFlags, ETeleportType Teleport = ETeleportType::None) override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override; // For Billboard
#endif

private:
	/** Billboard visualization component for this target */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JumpPad Target", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBillboardComponent> BillboardComponent;

	/** Icon used for the billboard component */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Jump Pad", meta = (AllowPrivateAccess = "true"))
	UTexture2D* BillboardIconTexture;
};