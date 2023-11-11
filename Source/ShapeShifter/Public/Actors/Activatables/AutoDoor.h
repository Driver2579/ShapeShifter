// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Activatables/Door.h"
#include "AutoDoor.generated.h"

UCLASS()
class SHAPESHIFTER_API AAutoDoor : public ADoor
{
	GENERATED_BODY()
public:
	AAutoDoor();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UBoxComponent* TriggerComponent;

	UFUNCTION()
	void OnTriggerOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnTriggerEndBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);
};