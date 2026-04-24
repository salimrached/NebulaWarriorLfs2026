// Fill out your copyright notice in the Description page of Project Settings.


#include "JetAdvancedComponent.h"

// Constructor
UJetAdvancedComponent::UJetAdvancedComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// BeginPlay
void UJetAdvancedComponent::BeginPlay()
{
	Super::BeginPlay();

	MissilesCount = MissilesCapacity;
	IdleRechargeTimer = IdleRechargeDelay;
}


// TryShootMissile
bool UJetAdvancedComponent::TryShootMissile()
{
	if (bRecharging || MissilesCount <= 0)
		return false;

	--MissilesCount;
	IdleRechargeTimer = IdleRechargeDelay;

	if (MissilesCount == 0)
	{
		bRecharging = true;
		RechargeDelayTimer = RechargeDelay;
		RechargeProgress = 0.f;
	}

	return true;
}


// Tick
void UJetAdvancedComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Idle regen: start refill from current count if player hasn't fired for a while
	if (!bRecharging && MissilesCount < MissilesCapacity)
	{
		IdleRechargeTimer -= DeltaTime;
		if (IdleRechargeTimer <= 0.f)
		{
			bRecharging = true;
			RechargeDelayTimer = 0.f;
			RechargeProgress = (float)MissilesCount / (float)MissilesCapacity;
		}
	}

	if (!bRecharging)
		return;

	if (RechargeDelayTimer > 0.f)
	{
		RechargeDelayTimer -= DeltaTime;
		return;
	}

	RechargeProgress = FMath::Min(RechargeProgress + DeltaTime / RechargeDuration, 1.f);

	if (RechargeProgress >= 1.f)
	{
		MissilesCount = MissilesCapacity;
		bRecharging = false;
		RechargeProgress = 0.f;
	}
}

