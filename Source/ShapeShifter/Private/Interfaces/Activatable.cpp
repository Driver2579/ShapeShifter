// Fill out your copyright notice in the Description page of Project Settings.

#include "Interfaces/Activatable.h"

void IActivatable::Activate()
{
	bActive = true;
}

void IActivatable::Deactivate()
{
	bActive = false;
}

bool IActivatable::IsActive() const
{
	return bActive;
}

void IActivatable::SetActive(const bool bNewActive)
{
	bActive = bNewActive;
}