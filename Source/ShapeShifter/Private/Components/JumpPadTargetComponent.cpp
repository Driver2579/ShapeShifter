// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/JumpPadTargetComponent.h"

#include "Actors/Jumppad.h"
#include "Components/BillboardComponent.h"

UJumpPadTargetComponent::UJumpPadTargetComponent()
{
	bWantsOnUpdateTransform = true;

	// Create a BillboardComponent for in-scene visualization
	BillboardComponent = CreateDefaultSubobject<UBillboardComponent>(TEXT("Billboard"));
	// Set the default icon texture
	static ConstructorHelpers::FObjectFinder<UTexture2D> DefaultIcon(TEXT("/Engine/EditorResources/S_TargetPoint.S_TargetPoint"));
	if (DefaultIcon.Succeeded())
	{
		BillboardIconTexture = DefaultIcon.Object;
	}
	else
	{
		BillboardIconTexture = nullptr;
	}
}

void UJumpPadTargetComponent::OnRegister()
{
	Super::OnRegister();

	if (!BillboardComponent)
	{
		return;
	}
	
	// BillboardComponent for in-scene visualization
	BillboardComponent->SetupAttachment(this);
	BillboardComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f)); 
	BillboardComponent->bIsEditorOnly = true;
	BillboardComponent->SetVisibility(true);
	BillboardComponent->SetHiddenInGame(true);

	// Dynamically load and set the default sprite
	if (BillboardIconTexture)
	{
		BillboardComponent->SetSprite(BillboardIconTexture);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to load texture: %s"), *BillboardIconTexture->GetName());
	}
	
	BillboardComponent->SetRelativeScale3D(FVector(0.5f));
	BillboardComponent->SetOpacityMaskRefVal(0.5f);
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

	// Normal in the direction of where to turn the JumpPad
	const FVector NewJumpPadDirection = GetRelativeLocation().GetSafeNormal();

	// The angle at which the JumpPad should be turned
	float Angle = FMath::Atan2(NewJumpPadDirection.Y, NewJumpPadDirection.X) * (180.0f / PI);

	// Removing a negative angle
	if (Angle < 0.0f) 
	{
		Angle += 360.0f;
	}

	// Set JumpPad Mesh rotation using Angle
	OwningJumpPad->GetMesh()->SetWorldRotation(FRotator(0.0f, Angle, 0.0f));
}

void UJumpPadTargetComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    if (BillboardComponent)
    {
        BillboardComponent->MarkRenderStateDirty();
    }
}

#endif