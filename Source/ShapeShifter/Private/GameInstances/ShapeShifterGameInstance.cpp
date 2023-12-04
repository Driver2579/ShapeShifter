// Fill out your copyright notice in the Description page of Project Settings.

#include "GameInstances/ShapeShifterGameInstance.h"

bool UShapeShifterGameInstance::IsAutoSaveAllowed() const
{
	return bAllowAutoSave;
}

void UShapeShifterGameInstance::SetAllowAutoSave(const bool bNewAllowAutoSave)
{
	bAllowAutoSave = bNewAllowAutoSave;
}

bool UShapeShifterGameInstance::WillEnableAllowAutoSaveOnOpenLevel() const
{
	return bEnableAllowAutoSaveOnOpenLevel;
}

void UShapeShifterGameInstance::SetDisableAllowAutoSaveOnOpenLevel(const bool bNewEnableAllowAutoSaveOnOpenLevel)
{
	bEnableAllowAutoSaveOnOpenLevel = bNewEnableAllowAutoSaveOnOpenLevel;
}