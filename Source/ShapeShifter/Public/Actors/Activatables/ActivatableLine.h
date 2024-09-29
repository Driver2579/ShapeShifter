// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Activatable.h"
#include "ActivatableLine.generated.h"

UCLASS()
class SHAPESHIFTER_API AActivatableLine : public AActor, public IActivatable
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	AActivatableLine();

	virtual void OnConstruction(const FTransform& Transform) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual bool IsActive() const override final;

	virtual void Activate() override;
	virtual void Deactivate() override;

private:
	UPROPERTY(VisibleAnywhere, Category="Components")
	TObjectPtr<class USplineMeshesComponent> SplineMeshesComponent;

	// Default Active state of the line
	UPROPERTY(EditAnywhere, Category="Activation")
	bool bActive = false;

	// The name of the color parameter in the materials of the SplineMeshComponents created by the SplineMeshesComponent
	UPROPERTY(EditDefaultsOnly, Category="Materials")
	FName MaterialParameterColorName;

	UPROPERTY(EditDefaultsOnly, Category="Materials")
	FLinearColor MaterialsActiveColor;

	UPROPERTY(EditDefaultsOnly, Category="Materials")
	FLinearColor MaterialsInactiveColor;

	void SetMaterialsColor(const FLinearColor& Color) const;
};