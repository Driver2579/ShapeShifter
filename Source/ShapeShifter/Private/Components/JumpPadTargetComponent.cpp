// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/JumpPadTargetComponent.h"
#include "Actors/Activatables/Jumppad.h"

UJumpPadTargetComponent::UJumpPadTargetComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	bWantsOnUpdateTransform = true;
}

#if WITH_EDITOR
void UJumpPadTargetComponent::OnUpdateTransform(EUpdateTransformFlags UpdateTransformFlags, ETeleportType Teleport)
{
	Super::OnUpdateTransform(UpdateTransformFlags, Teleport);

	AJumpPad* OwnerActor = Cast<AJumpPad>(GetOwner());

	if (!OwnerActor) 
	{
		return;
	}

	const FVector NewDirection = GetRelativeLocation().GetSafeNormal();
	float Angle = FMath::Atan2(NewDirection.Y, NewDirection.X) * (180.0f / PI);
	if (Angle < 0.0f) 
	{
		Angle += 360.0f;
	}

	OwnerActor->GetMesh()->SetWorldRotation(FRotator(0.0f, Angle, 0.0f));
}
#endif

void UJumpPadTargetComponent::BeginPlay()
{
	Super::BeginPlay();
	
}
