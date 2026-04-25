// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "JetAdvancedComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NEBULAWARRIOR_API UJetAdvancedComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UJetAdvancedComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Attempts to shoot a missile. Returns true if shots remain.
	UFUNCTION(BlueprintCallable, Category = "Missiles")
	bool TryShootMissile();

	// Returns remaining missiles as a 0.0-1.0 ratio for a UMG progress bar.
	UFUNCTION(BlueprintPure, Category = "Missiles")
	float GetEnergyPercent() const
	{
		if (bRecharging)
			return RechargeProgress;
		return (float)MissilesCount / (float)MissilesCapacity;
	}

	// Total missiles before recharge is required.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Missiles")
	int32 MissilesCapacity = 33;

	UPROPERTY(BlueprintReadOnly, Category = "Missiles")
	int32 MissilesCount = 33;

	// Seconds to wait after running out before the bar recharges.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Missiles")
	float RechargeDelay = 1.5f;

	// How many seconds the bar takes to go from empty to full.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Missiles")
	float RechargeDuration = 3.f;

	// Seconds after the last shot with no firing before idle refill starts (from current count).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Missiles")
	float IdleRechargeDelay = 5.f;

private:
	bool bRecharging = false;
	float RechargeDelayTimer = 0.f;
	float RechargeProgress = 0.f;
	float IdleRechargeTimer = 0.f;
};
