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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* BaseMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* LeftDoorMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* RightDoorMeshComponent;

private:
	UPROPERTY(EditAnywhere, Category = "Activation")
	bool bActive = false;

	// Start and end positions for each door
	FVector StartLeftDoorLocation;
	FVector StartRightDoorLocation;
	FVector EndLeftDoorLocation;
	FVector EndRightDoorLocation;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float MoveOffset = 60;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float MoveSpeed = 75;
};