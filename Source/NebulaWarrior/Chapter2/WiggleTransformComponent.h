// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WiggleTransformComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOscillateTimer, float, Delta);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NEBULAWARRIOR_API UWiggleTransformComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UWiggleTransformComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void BeginPlay() override;

public:

	// Default amplitude used when calling Oscillate with no override
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wiggle")
	float Amplitude = 5.f;

	// Default frequency used when calling Oscillate with no override
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wiggle")
	float Frequency = 2.f;

	// Returns a per-frame delta. Pass a dedicated float variable per axis as PreviousValue
	// so multiple axes don't share state. Feed result into AddLocalRotation / AddLocalOffset.
	UFUNCTION(BlueprintCallable, Category = "Wiggle")
	float Oscillate(float InAmplitude, float InFrequency, UPARAM(ref) float& PreviousValue);

	// Fired on each timer tick — bind in Blueprint to apply the delta to any component/axis
	UPROPERTY(BlueprintAssignable, Category = "Wiggle")
	FOnOscillateTimer OnOscillateTimer;

	// Start a fixed-interval oscillation independent of frame rate
	UFUNCTION(BlueprintCallable, Category = "Wiggle")
	void StartOscillateTimer(float Interval, float InAmplitude, float InFrequency);

	// Stop the timer-based oscillation
	UFUNCTION(BlueprintCallable, Category = "Wiggle")
	void StopOscillateTimer();

private:
	float ElapsedTime = 0.f;
	float TimerAmplitude = 5.f;
	float TimerFrequency = 2.f;
	float TimerPreviousValue = 0.f;
	FTimerHandle OscillateTimerHandle;
	void OscillateTimerTick();
};
