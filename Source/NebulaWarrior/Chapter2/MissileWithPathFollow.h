// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MissileWithPathFollow.generated.h"

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
	// SplinePathComponent - set from Blueprint on BeginPlay
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Path")
	class USplineComponent* SplinePathComponent;

	// SphereCollisionComponent
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision")
	class USphereComponent* SphereCollisionComponent;

	// When true the missile follows the spline; false = free flight
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

	// How long the missile flies freely before blending (0 = instant spline follow)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FreeFlight")
	float FreeFlightDuration = 1.0f;

	// How long the lerp transition from free flight to spline follow lasts
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FreeFlight")
	float BlendDuration = 1.0f;

	// How long the missile follows the spline before locking onto a target (Phase 3)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Path")
	float SplineFollowDuration = 1.0f;

	// Target Follow (Phase 4)
	// Array of potential targets - set from Blueprint (e.g. GetAllActorsOfClass)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TargetFollow")
	TArray<AActor*> TargetActors;

	// When true the missile will lock onto the closest target after SplineFollowDuration
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TargetFollow")
	bool bFollowTarget = true;

	// Speed at which the missile moves toward the locked target (units/sec)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TargetFollow")
	float TargetFollowSpeed = 24000.0f;

	// How fast the missile rotates to face the locked target (degrees/sec)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TargetFollow")
	float TargetTurnSpeed = 5.0f;

	// The locked target selected at the start of Phase 4 (closest from TargetActors)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TargetFollow")
	AActor* LockedTarget = nullptr;

private:
	// Captured on BeginPlay from the actor's forward vector
	FVector FreeFlightVelocity = FVector::ZeroVector;

	// Accumulates time since spawn to drive phase transitions
	float ElapsedTime = 0.0f;

	// Seeds SplineDistance to the nearest point and captures Y/Z offsets
	void InitializeSplineFollow();

	// Picks the closest actor from TargetActors and stores it in LockedTarget
	void InitializeTargetFollow();

	void TickFreeFlight(float DeltaTime);
	void TickBlend(float DeltaTime);
	void TickSplineFollow(float DeltaTime);
	void TickTargetFollow(float DeltaTime);
};
