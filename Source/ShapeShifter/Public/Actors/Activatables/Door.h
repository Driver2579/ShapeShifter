// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Activatable.h"
#include "Door.generated.h"

UCLASS()
class SHAPESHIFTER_API ADoor : public AActor, public IActivatable
{
	GENERATED_BODY()

public:	
	ADoor();

	virtual bool IsActive() const override final;

	virtual void Activate() override;
	virtual void Deactivate() override;

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UStaticMeshComponent* BaseMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* LeftDoorMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* RightDoorMeshComponent;

private:
	UPROPERTY(EditAnywhere, Category = "Activation")
	bool bActive = false;

	// Start and end locations for each door
	FVector StartLeftDoorLocation;
	FVector StartRightDoorLocation;
	FVector EndLeftDoorLocation;
	FVector EndRightDoorLocation;

	UPROPERTY(EditDefaultsOnly, Category = "Movement", meta=(ClampMin = 0))
	float MoveOffset = 60;

	UPROPERTY(EditDefaultsOnly, Category = "Movement", meta = (ClampMin = 0))
	float MoveSpeed = 75;
};