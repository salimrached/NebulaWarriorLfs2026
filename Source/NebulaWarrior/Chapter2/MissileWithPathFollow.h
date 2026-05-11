// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MissileWithPathFollow.generated.h"

UENUM(BlueprintType)
enum class EMissileMode : uint8
{
	FreeFlight    UMETA(DisplayName = "Free Flight"),
	PathFollow    UMETA(DisplayName = "Path Follow"),
	TargetFollow  UMETA(DisplayName = "Target Follow")
};

UCLASS()
class NEBULAWARRIOR_API AMissileWithPathFollow : public AActor
{
	GENERATED_BODY()
	
public:	
	AMissileWithPathFollow();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

public:	
	// Missile mode - set in Blueprint BeginPlay to drive the Switch on EMissileMode logic
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Missile")
	EMissileMode MissileMode = EMissileMode::FreeFlight;

	// Sets MissileMode to a random value from the EMissileMode enum
	UFUNCTION(BlueprintCallable, Category = "Missile")
	void SetRandomMissileMode();

	// SplinePathComponent - set from Blueprint on BeginPlay
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Path")
	class USplineComponent* SplinePathComponent;

	// SphereCollisionComponent
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision")
	class USphereComponent* SphereCollisionComponent;

	// --- Option 1: Straight Flight -> Path Follow ---
	// Set bFollowSpline = true, bFollowTarget = false

	// When true the missile transitions to spline follow after free flight
	// Mutually exclusive with bFollowTarget
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Path")
	bool bFollowSpline = false;

	// Current distance along the spline (seeded to nearest point at spawn)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Path")
	float SplineDistance = 0.0f;

	// Speed along the spline (units/sec) - match player speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Path")
	float SplineSpeed = 24000.0f;

	// Lateral (Y) offset from the spline center, captured at spawn from the missile's world position
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Path")
	float SplineOffsetY = 0.0f;

	// Height (Z) offset from the spline center, captured at spawn from the missile's world position
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Path")
	float SplineOffsetZ = 0.0f;

	// Speed during free-flight (units/sec)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FreeFlight")
	float FreeFlightSpeed = 24000.0f;

	// How long the missile flies freely before transitioning (0 = instant transition)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FreeFlight")
	float FreeFlightDuration = 1.0f;

	// How long the lerp transition from free flight to spline follow lasts (Option 1 only)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FreeFlight")
	float BlendDuration = 1.0f;

	// --- Option 2: Straight Flight -> Target Follow ---
	// Set bFollowTarget = true, bFollowSpline = false

	// Array of potential targets - set from Blueprint (e.g. GetAllActorsOfClass)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TargetFollow")
	TArray<AActor*> TargetActors;

	// When true the missile transitions to target follow after free flight
	// Mutually exclusive with bFollowSpline
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TargetFollow")
	bool bFollowTarget = false;

	// Speed at which the missile moves toward the locked target (units/sec)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TargetFollow")
	float TargetFollowSpeed = 24000.0f;

	// How fast the missile rotates to face the locked target (degrees/sec)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TargetFollow")
	float TargetTurnSpeed = 5.0f;

	// The randomly selected locked target (set at start of target follow phase)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TargetFollow")
	AActor* LockedTarget = nullptr;

private:
	// Captured on BeginPlay from the actor's forward vector
	FVector FreeFlightVelocity = FVector::ZeroVector;

	// Accumulates time since spawn to drive phase transitions
	float ElapsedTime = 0.0f;

	// Guards InitializeSplineFollow so it only runs once
	bool bSplineInitialized = false;

	// Seeds SplineDistance to the nearest point and captures Y/Z offsets
	void InitializeSplineFollow();

	// Picks the closest actor from TargetActors and stores it in LockedTarget
	void InitializeTargetFollow();

	void TickFreeFlight(float DeltaTime);
	void TickBlend(float DeltaTime);
	void TickSplineFollow(float DeltaTime);
	void TickTargetFollow(float DeltaTime);
};
