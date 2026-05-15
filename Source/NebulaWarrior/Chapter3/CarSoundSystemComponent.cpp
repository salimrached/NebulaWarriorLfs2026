// Fill out your copyright notice in the Description page of Project Settings.

#include "CarSoundSystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "Components/AudioComponent.h"

// Constructor
UCarSoundSystemComponent::UCarSoundSystemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	// Default gear thresholds:
	// Gear 0 — Idle:   0    to  999
	// Gear 1:          1000 to  3799
	// Gear 2:          3800 to  6599
	// Gear 3:          6600 to  9399
	// Gear 4:          9400 to  12199
	// Gear 5:          12200 to 15000
	GearThresholds = {
		{ 0.f,     999.f   },
		{ 1000.f,  3799.f  },
		{ 3800.f,  6599.f  },
		{ 6600.f,  9399.f  },
		{ 9400.f,  12199.f },
		{ 12200.f, 15000.f }
	};
}


// BeginPlay
void UCarSoundSystemComponent::BeginPlay()
{
	Super::BeginPlay();

	if (ACharacter* OwnerChar = Cast<ACharacter>(GetOwner()))
	{
		MovementComponent = OwnerChar->GetCharacterMovement();
	}
}


// TickComponent
void UCarSoundSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!MovementComponent)
	{
		return;
	}

	CurrentVelocity = MovementComponent->Velocity.Size();

	const int32 NewGear = ResolveGear(CurrentVelocity);
	if (NewGear != CurrentGear)
	{
		const int32 OldGear = CurrentGear;
		CurrentGear = NewGear;
		OnGearChanged.Broadcast(OldGear, NewGear);
	}
}


// ResolveGear — uses hysteresis to prevent flickering at gear boundaries
int32 UCarSoundSystemComponent::ResolveGear(float Speed) const
{
	if (GearThresholds.IsEmpty())
	{
		return 0;
	}

	// Walk top-down and return the highest gear whose entry threshold is met.
	// For downshifts we lower the current gear's MinSpeed by HysteresisBuffer
	// so the car has to slow down a bit more before dropping a gear.
	for (int32 i = GearThresholds.Num() - 1; i >= 0; --i)
	{
		const float EffectiveMin = (i == CurrentGear && i > 0)
			? GearThresholds[i].MinSpeed - HysteresisBuffer
			: GearThresholds[i].MinSpeed;

		if (Speed >= EffectiveMin)
		{
			return i;
		}
	}

	return 0;
}

