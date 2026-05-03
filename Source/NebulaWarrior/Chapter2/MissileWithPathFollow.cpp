// Fill out your copyright notice in the Description page of Project Settings.


#include "MissileWithPathFollow.h"
#include "Components/SplineComponent.h"
#include "Components/SphereComponent.h"

// Constructor
AMissileWithPathFollow::AMissileWithPathFollow()
{
	PrimaryActorTick.bCanEverTick = true;
	//SphereCollisionComponent
	SphereCollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollisionComponent"));
	RootComponent = SphereCollisionComponent;

}

// BeginPlay
void AMissileWithPathFollow::BeginPlay()
{
	Super::BeginPlay();

	// Capture the forward direction once so free-flight is independent of later rotation changes.
	// Note: SplinePathComponent and TargetActors are set by Blueprint BeginPlay which runs AFTER
	// C++ BeginPlay, so all initialization that depends on them is handled in Tick instead.
	FreeFlightVelocity = GetActorForwardVector() * FreeFlightSpeed;
}

// Tick
void AMissileWithPathFollow::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ElapsedTime += DeltaTime;

	// Phase 1 - Free Flight
	if (ElapsedTime < FreeFlightDuration)
	{
		TickFreeFlight(DeltaTime);
		return;
	}

	// Transition into blend: initialize spline on the first tick after free flight ends
	if (!bFollowSpline && SplinePathComponent)
	{
		InitializeSplineFollow();
		bFollowSpline = true;
	}

	// Phase 2 - Blend
	if (ElapsedTime < FreeFlightDuration + BlendDuration)
	{
		TickBlend(DeltaTime);
		return;
	}

	// Phase 3 - Pure Spline Follow
	if (ElapsedTime < FreeFlightDuration + BlendDuration + SplineFollowDuration)
	{
		TickSplineFollow(DeltaTime);
		return;
	}

	// Transition into target follow: pick closest target on the first tick after Phase 3 ends
	if (bFollowTarget && LockedTarget == nullptr)
	{
		InitializeTargetFollow();
	}

	// Phase 4 - Target Follow
	if (bFollowTarget && LockedTarget)
	{
		TickTargetFollow(DeltaTime);
		return;
	}

	// Fallback: keep following spline if target follow is disabled or no target was found
	TickSplineFollow(DeltaTime);
}

// TickFreeFlight - Phase 1: move along the missile's initial forward vector
void AMissileWithPathFollow::TickFreeFlight(float DeltaTime)
{
	const FVector NewLocation = GetActorLocation() + FreeFlightVelocity * DeltaTime;
	SetActorLocation(NewLocation);
}

// TickBlend - Phase 2: lerp position and rotation from free flight to spline follow
void AMissileWithPathFollow::TickBlend(float DeltaTime)
{
	if (!SplinePathComponent)
	{
		return;
	}

	// Advance the spline target while blending so it doesn't freeze in place
	const float SplineLength = SplinePathComponent->GetSplineLength();
	SplineDistance = FMath::Fmod(SplineDistance + SplineSpeed * DeltaTime, SplineLength);

	// Compute spline target with offsets
	const FVector SplinePos    = SplinePathComponent->GetLocationAtDistanceAlongSpline(SplineDistance, ESplineCoordinateSpace::World);
	const FRotator SplineRot   = SplinePathComponent->GetRotationAtDistanceAlongSpline(SplineDistance, ESplineCoordinateSpace::World);
	const FVector SplineRight  = SplineRot.RotateVector(FVector::RightVector);
	const FVector SplineUp     = SplineRot.RotateVector(FVector::UpVector);
	const FVector SplineTarget = SplinePos + SplineRight * SplineOffsetY + SplineUp * SplineOffsetZ;

	// Alpha goes 0 -> 1 over BlendDuration
	const float BlendElapsed = ElapsedTime - FreeFlightDuration;
	const float Alpha = FMath::Clamp(BlendElapsed / BlendDuration, 0.0f, 1.0f);

	// Lerp location, Slerp rotation
	const FVector FreeFlightPos = GetActorLocation() + FreeFlightVelocity * DeltaTime;
	const FVector FinalLocation = FMath::Lerp(FreeFlightPos, SplineTarget, Alpha);
	const FRotator FinalRotation = FRotator(FMath::Lerp(FQuat(GetActorRotation()), FQuat(SplineRot), Alpha));

	SetActorLocation(FinalLocation);
	SetActorRotation(FinalRotation);
}

// InitializeSplineFollow - seeds SplineDistance to the nearest point on the spline
// and captures the missile's lateral (Y) and height (Z) offset in the spline's local frame
void AMissileWithPathFollow::InitializeSplineFollow()
{
	const FVector SpawnLocation = GetActorLocation();

	// Find the nearest point on the spline (works correctly on closed splines)
	const float InputKey = SplinePathComponent->FindInputKeyClosestToWorldLocation(SpawnLocation);
	SplineDistance = SplinePathComponent->GetDistanceAlongSplineAtSplineInputKey(InputKey);

	// Compute the offset of the spawn location relative to the spline point in the spline's local frame
	const FVector SplinePos = SplinePathComponent->GetLocationAtDistanceAlongSpline(SplineDistance, ESplineCoordinateSpace::World);
	const FRotator SplineRot = SplinePathComponent->GetRotationAtDistanceAlongSpline(SplineDistance, ESplineCoordinateSpace::World);
	const FVector SplineRight = SplineRot.RotateVector(FVector::RightVector);
	const FVector SplineUp    = SplineRot.RotateVector(FVector::UpVector);
	const FVector Delta       = SpawnLocation - SplinePos;

	SplineOffsetY = FVector::DotProduct(Delta, SplineRight);
	SplineOffsetZ = FVector::DotProduct(Delta, SplineUp);
}

// TickSplineFollow - advances along the spline and applies Y/Z offsets
void AMissileWithPathFollow::TickSplineFollow(float DeltaTime)
{
	if (!SplinePathComponent)
	{
		return;
	}

	// Advance distance, wrapping around the closed spline
	const float SplineLength = SplinePathComponent->GetSplineLength();
	SplineDistance = FMath::Fmod(SplineDistance + SplineSpeed * DeltaTime, SplineLength);

	// Get spline transform at current distance
	const FVector SplinePos   = SplinePathComponent->GetLocationAtDistanceAlongSpline(SplineDistance, ESplineCoordinateSpace::World);
	const FRotator SplineRot  = SplinePathComponent->GetRotationAtDistanceAlongSpline(SplineDistance, ESplineCoordinateSpace::World);
	const FVector SplineRight = SplineRot.RotateVector(FVector::RightVector);
	const FVector SplineUp    = SplineRot.RotateVector(FVector::UpVector);

	// Apply lateral and height offsets in the spline's local frame
	const FVector FinalLocation = SplinePos + SplineRight * SplineOffsetY + SplineUp * SplineOffsetZ;

	SetActorLocation(FinalLocation);
	SetActorRotation(SplineRot);
}

// InitializeTargetFollow - picks the farthest actor from TargetActors and locks onto it
void AMissileWithPathFollow::InitializeTargetFollow()
{
	if (TargetActors.Num() == 0)
	{
		return;
	}

	const FVector MissileLocation = GetActorLocation();
	AActor* Farthest = nullptr;
	float FarthestDistSq = -1.0f;

	for (AActor* Candidate : TargetActors)
	{
		if (!IsValid(Candidate))
		{
			continue;
		}

		const float DistSq = FVector::DistSquared(MissileLocation, Candidate->GetActorLocation());
		if (DistSq > FarthestDistSq)
		{
			FarthestDistSq = DistSq;
			Farthest = Candidate;
		}
	}

	LockedTarget = Farthest;
}

// TickTargetFollow - Phase 4: home in on the locked target
void AMissileWithPathFollow::TickTargetFollow(float DeltaTime)
{
	// If the target was destroyed mid-flight, fall back to spline
	if (!IsValid(LockedTarget))
	{
		LockedTarget = nullptr;
		TickSplineFollow(DeltaTime);
		return;
	}

	const FVector MissileLocation = GetActorLocation();
	const FVector ToTarget        = (LockedTarget->GetActorLocation() - MissileLocation).GetSafeNormal();

	// Smoothly rotate toward the target using RInterpTo
	const FRotator CurrentRotation = GetActorRotation();
	const FRotator DesiredRotation = ToTarget.Rotation();
	const FRotator NewRotation     = FMath::RInterpTo(CurrentRotation, DesiredRotation, DeltaTime, TargetTurnSpeed);

	// Move forward along the new rotation
	const FVector NewLocation = MissileLocation + NewRotation.Vector() * TargetFollowSpeed * DeltaTime;

	SetActorLocation(NewLocation);
	SetActorRotation(NewRotation);
}

