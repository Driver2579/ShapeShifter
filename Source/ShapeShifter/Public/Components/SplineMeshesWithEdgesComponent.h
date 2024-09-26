// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SplineMeshesComponent.h"
#include "SplineMeshesWithEdgesComponent.generated.h"

/**
 * The same as SplineMeshesComponent but with two additional meshes at the edges of the spline (one at the start and one
 * at the end).
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SHAPESHIFTER_API USplineMeshesWithEdgesComponent : public USplineMeshesComponent
{
	GENERATED_BODY()

	// TODO: Implement this class.
};