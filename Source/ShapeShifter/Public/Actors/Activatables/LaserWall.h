// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Activatable.h"
#include "LaserWall.generated.h"

/**
 * A group of Lasers combined into one class to simply manage them on levels. This class is empty by default. Attach
 * ChildActorComponents with Laser ChildActorClass to the AttachPointComponent to make it work.
 */
UCLASS()
class SHAPESHIFTER_API ALaserWall : public AActor, public IActivatable
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ALaserWall();

	virtual bool IsActive() const override;

	virtual void Activate() override;
	virtual void Deactivate() override;

protected:
	virtual void PostInitializeComponents() override;

	// All ChildActorComponents with Laser ChildActorClass should be attached to here
	UPROPERTY(VisibleAnywhere, Category = "Components")
	USceneComponent* AttachPointComponent;

private:
	TArray<class ALaser*> ChildLasers;

	// Default Active state of all attached Lasers
	UPROPERTY(EditAnywhere, Category = "Activation")
	bool bActive = true;
};