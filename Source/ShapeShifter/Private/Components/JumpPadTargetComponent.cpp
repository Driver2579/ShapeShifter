// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/JumpPadTargetComponent.h"

#include "Actors/Activatables/Jumppad.h"

UJumpPadTargetComponent::UJumpPadTargetComponent()
{
	bWantsOnUpdateTransform = true;
}

#if WITH_EDITOR
void UJumpPadTargetComponent::OnUpdateTransform(EUpdateTransformFlags UpdateTransformFlags, ETeleportType Teleport)
{
	Super::OnUpdateTransform(UpdateTransformFlags, Teleport);

	const AJumpPad* OwningJumpPad = GetOwner<AJumpPad>();

	if (!OwningJumpPad)
	{
		UE_LOG(LogTemp, Error, TEXT("UJumpPadTargetComponent:OnUpdateTransform: OwningJumpPad is not valid"));

		return;
	}

	// Normal in the direction of where to turn the jumppad
	const FVector NewJumpPadDirection = GetRelativeLocation().GetSafeNormal();

	// The angle at which the jumppad should be turned
	float Angle = FMath::Atan2(NewJumpPadDirection.Y, NewJumpPadDirection.X) * (180.0f / PI);

	// Removing a negative angle
	if (Angle < 0.0f) 
	{
		Angle += 360.0f;
	}

	// Set JumpPad Mesh rotation using Angle
	OwningJumpPad->GetMesh()->SetWorldRotation(FRotator(0.0f, Angle, 0.0f));
}
#endif
