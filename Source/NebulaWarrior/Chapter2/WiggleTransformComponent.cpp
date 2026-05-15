// Fill out your copyright notice in the Description page of Project Settings.

#include "WiggleTransformComponent.h"

// Constructor
UWiggleTransformComponent::UWiggleTransformComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


// BeginPlay
void UWiggleTransformComponent::BeginPlay()
{
	Super::BeginPlay();
}


// TickComponent — advances internal time so Oscillate always has a continuous wave
void UWiggleTransformComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	ElapsedTime += DeltaTime;
}


// Each axis passes its own PreviousValue so multiple calls per tick don't share state
float UWiggleTransformComponent::Oscillate(float InAmplitude, float InFrequency, float& PreviousValue)
{
	const float CurrentValue = FMath::Sin(ElapsedTime * InFrequency * 2.f * PI) * InAmplitude;
	const float Delta = CurrentValue - PreviousValue;
	PreviousValue = CurrentValue;
	return Delta;
}


// Start fixed-interval oscillation independent of frame rate
void UWiggleTransformComponent::StartOscillateTimer(float Interval, float InAmplitude, float InFrequency)
{
	TimerAmplitude = InAmplitude;
	TimerFrequency = InFrequency;
	TimerPreviousValue = 0.f;

	GetWorld()->GetTimerManager().SetTimer(
		OscillateTimerHandle,
		this,
		&UWiggleTransformComponent::OscillateTimerTick,
		Interval,
		true
	);
}


// Stop the timer-based oscillation
void UWiggleTransformComponent::StopOscillateTimer()
{
	GetWorld()->GetTimerManager().ClearTimer(OscillateTimerHandle);
	TimerPreviousValue = 0.f;
}


// Internal timer callback — broadcasts delta to Blueprint
void UWiggleTransformComponent::OscillateTimerTick()
{
	const float CurrentValue = FMath::Sin(ElapsedTime * TimerFrequency * 2.f * PI) * TimerAmplitude;
	const float Delta = CurrentValue - TimerPreviousValue;
	TimerPreviousValue = CurrentValue;
	OnOscillateTimer.Broadcast(Delta);
}

