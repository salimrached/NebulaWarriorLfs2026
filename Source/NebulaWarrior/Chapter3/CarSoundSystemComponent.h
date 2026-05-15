// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CarSoundSystemComponent.generated.h"

class UCharacterMovementComponent;
class UAudioComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGearChanged, int32, OldGear, int32, NewGear);

USTRUCT(BlueprintType)
struct FGearThreshold
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gear")
	float MinSpeed = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gear")
	float MaxSpeed = 0.f;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NEBULAWARRIOR_API UCarSoundSystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCarSoundSystemComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void BeginPlay() override;

public:

	// Fired once when the gear changes — bind this in Blueprint to trigger your Timeline
	UPROPERTY(BlueprintAssignable, Category = "Car Sound")
	FOnGearChanged OnGearChanged;

	// The MetaSound AudioComponent playing the engine sound
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Car Sound")
	TObjectPtr<UAudioComponent> CarAudioComponent;

	// Gear speed thresholds — editable in BP defaults
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Car Sound")
	TArray<FGearThreshold> GearThresholds;

	// Buffer below a gear's MinSpeed before shifting back down (prevents flickering)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Car Sound")
	float HysteresisBuffer = 150.f;

	UPROPERTY(BlueprintReadOnly, Category = "Car Sound")
	int32 CurrentGear = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Car Sound")
	float CurrentVelocity = 0.f;

private:
	TObjectPtr<UCharacterMovementComponent> MovementComponent;

	int32 ResolveGear(float Speed) const;
};
