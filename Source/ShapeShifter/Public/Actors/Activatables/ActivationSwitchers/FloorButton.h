// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActivationSwitcher.h"
#include "Interfaces/Activatable.h"
#include "FloorButton.generated.h"

UCLASS()
class SHAPESHIFTER_API AFloorButton : public AActivationSwitcher, public IActivatable
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AFloorButton();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual bool IsActive() const override;

	virtual void Activate() override;
	virtual void Deactivate() override;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* BaseMeshComponent;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* ButtonMeshComponent;

	// This is used to generate overlap events with any collision shape. This component should not be visible.
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* ButtonTriggerComponent;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnButtonTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnButtonTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	void SetupComponents();

	bool bActive = false;

	// ButtonMeshComponent location when pressed
	FVector ButtonMeshActiveLocation;

	// ButtonMeshComponent location when released
	FVector ButtonMeshInactiveLocation;

	UPROPERTY(EditDefaultsOnly, Category = "Button Movement")
	float MoveOffset = 5;

	UPROPERTY(EditDefaultsOnly, Category = "Button Movement")
	float MoveSpeed = 25;
};