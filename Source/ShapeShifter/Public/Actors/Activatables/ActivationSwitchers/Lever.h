// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActivationSwitcher.h"
#include "Interfaces/Activatable.h"
#include "Lever.generated.h"

class UBoxComponent;

UCLASS()
class SHAPESHIFTER_API ALever : public AActivationSwitcher, public IActivatable
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ALever();

	virtual void OnConstruction(const FTransform& Transform) override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual bool IsActive() const override;

	virtual void Activate() override;
	virtual void Deactivate() override;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* BaseMeshComponent;
	
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* LeverMeshComponent;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UBoxComponent* ActivateZoneComponent;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UBoxComponent* DeactivateZoneComponent;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnLeverMeshHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

private:
	UPROPERTY(EditAnywhere, Category = "Activation")
	bool bActive = false;

	void OnActiveSwitch();

	// Velocity needed for physics component to activate or deactivate the lever
	UPROPERTY(EditDefaultsOnly, Category = "Activation")
	float VelocityToSwitchActivation = 10;

	bool bRotateLeverMesh = false;

	UPROPERTY(EditDefaultsOnly, Category = "Lever Rotation")
	FRotator LeverMeshActiveRotation = FRotator(0, 0, 110);

	UPROPERTY(EditDefaultsOnly, Category = "Lever Rotation")
	FRotator LeverMeshInactiveRotation = FRotator(0, 0, 70);

	UPROPERTY(EditDefaultsOnly, Category = "Lever Rotation")
	float RotationSpeed = 25;
};