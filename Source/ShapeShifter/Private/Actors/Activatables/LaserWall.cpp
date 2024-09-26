// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/Activatables/LaserWall.h"

#include "Actors/Activatables/Laser.h"

ALaserWall::ALaserWall()
{
 	AttachPointComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Attach Point"));
	RootComponent = AttachPointComponent;
}

void ALaserWall::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	/**
	 * We're doing it here instead of BeginPlay to make sure we set bActive in Lasers before they will call BeginPlay.
	 * We need it because Lasers spawn beams in BeginPlay and we can't make sure that BeginPlay of this class will be
	 * called before or after Lasers BeginPlay. This isn't a big issue but this could cause fake error logs because
	 * of calling SetActive before Lasers beams would spawn. So the solution is to disable modifying beams until
	 * BeginPlay was called and call SetActive for Lasers before BeginPlay. This will only set a bActive value for the
	 * Lasers and they're going to manage active state by themselves in the BeginPlay.
	 */

	// Iterate through all attached components
	for (auto ChildActorComponent : AttachPointComponent->GetAttachChildren())
	{
#if DO_ENSURE
		// Check if attached component is a ChildActorComponent
		if (!ensure(IsValid(ChildActorComponent)) || !ensure(ChildActorComponent.IsA<UChildActorComponent>()))
		{
			return;
		}
#endif

		const UChildActorComponent* CastedChildActorComponent = CastChecked<UChildActorComponent>(ChildActorComponent);

#if DO_ENSURE
		const bool bChildActorValid = ensure(IsValid(CastedChildActorComponent->GetChildActor()));
		const bool bChildActorIsLaser = ensure(CastedChildActorComponent->GetChildActor()->IsA<ALaser>());

		if (!bChildActorValid || !bChildActorIsLaser)
		{
			return;
		}
#endif

		// Get ChildLaser from ChildActorComponent
		ALaser* ChildLaser = CastChecked<ALaser>(CastedChildActorComponent->GetChildActor());

		// Add ChildLaser to ChildLasers array
		ChildLasers.Add(ChildLaser);

		// SetActive to ChildLaser now to not iterate through them again
		ChildLaser->SetActive(bActive);
	}
}

bool ALaserWall::IsActive() const
{
	return bActive;
}

void ALaserWall::Activate()
{
	bActive = true;

	// Activate all ChildLasers
	for (const TWeakObjectPtr<ALaser> ChildLaser : ChildLasers)
	{
		ChildLaser->Activate();
	}
}

void ALaserWall::Deactivate()
{
	bActive = false;

	// Deactivate all ChildLasers
	for (const TWeakObjectPtr<ALaser> ChildLaser : ChildLasers)
	{
		ChildLaser->Deactivate();
	}
}