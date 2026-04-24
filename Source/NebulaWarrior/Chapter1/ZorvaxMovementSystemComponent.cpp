// Fill out your copyright notice in the Description page of Project Settings.

#include "ZorvaxMovementSystemComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Math/UnrealMathUtility.h"

// Constructor
UZorvaxMovementSystemComponent::UZorvaxMovementSystemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

// BeginPlay
void UZorvaxMovementSystemComponent::BeginPlay()
{
	Super::BeginPlay();

	// Cache the player pawn at start
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC)
	{
		PlayerPawn = PC->GetPawn();
	}

	// Initialise PreviousX to the owner's starting X so the first tick delta is zero
	if (AActor* Owner = GetOwner())
	{
		PreviousX = Owner->GetActorLocation().X;
	}
}

// TickComponent
void UZorvaxMovementSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	switch (MovementPattern)
	{
	case EZorvaxMovementPattern::TrackHorizontally:
		TickTrackHorizontally(DeltaTime);
		break;

	case EZorvaxMovementPattern::None:
	default:
		break;
	}

	// Calculate velocity, update animation direction
	UpdateXVelocity(DeltaTime);
	UpdateAnimationDirection(XVelocity);
}

void UZorvaxMovementSystemComponent::TickTrackHorizontally(float DeltaTime)
{
	if (!PlayerPawn)
	{
		return;
	}

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	FVector OwnerLocation = Owner->GetActorLocation();
	FVector PlayerLocation = PlayerPawn->GetActorLocation();

	// Target X is the player's X plus any desired offset
	float TargetX = PlayerLocation.X + HorizontalOffset;

	// Clamp target X if a max tracking distance is set
	if (MaxTrackingDistance > 0.0f)
	{
		TargetX = FMath::Clamp(TargetX, PlayerLocation.X - MaxTrackingDistance, PlayerLocation.X + MaxTrackingDistance);
	}

	// Smoothly interpolate the enemy's X toward the target X — Y and Z are untouched
	float NewX = FMath::FInterpTo(OwnerLocation.X, TargetX, DeltaTime, TrackingSpeed);

	Owner->SetActorLocation(FVector(NewX, OwnerLocation.Y, OwnerLocation.Z));
}

void UZorvaxMovementSystemComponent::UpdateXVelocity(float DeltaTime)
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	float CurrentX = Owner->GetActorLocation().X;
	XVelocity = (CurrentX - PreviousX) / DeltaTime;
	PreviousX = CurrentX;
}

void UZorvaxMovementSystemComponent::UpdateAnimationDirection(float InXVelocity)
{
	if (FMath::Abs(InXVelocity) < MovementDeadzone)
	{
		return;
	}

	if (InXVelocity > 0.0f)
	{
		PlayRight();
	}
	else if (InXVelocity < 0.0f)
	{
		PlayLeft();
	}
}
