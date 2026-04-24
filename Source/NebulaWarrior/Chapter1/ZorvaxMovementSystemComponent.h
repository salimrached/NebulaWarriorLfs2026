// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ZorvaxMovementSystemComponent.generated.h"

UENUM(BlueprintType)
enum class EZorvaxMovementPattern : uint8
{
	None			UMETA(DisplayName = "None"),
	TrackHorizontally	UMETA(DisplayName = "Track Horizontally")
};

UCLASS( Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NEBULAWARRIOR_API UZorvaxMovementSystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UZorvaxMovementSystemComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// The active movement pattern for Zorvax
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zorvax Movement")
	EZorvaxMovementPattern MovementPattern = EZorvaxMovementPattern::TrackHorizontally;

	// How fast Zorvax interpolates toward the player's X position
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zorvax Movement")
	float TrackingSpeed = 3.0f;

	// Maximum allowed horizontal distance from the player (0 = unlimited)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zorvax Movement")
	float MaxTrackingDistance = 0.0f;

	// Optional fixed X offset from the player's position (positive = right of player)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zorvax Movement")
	float HorizontalOffset = 0.0f;

	// Minimum X velocity (units/sec) required to trigger a direction change — avoids jitter when nearly still
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zorvax Animation")
	float MovementDeadzone = 10.0f;

	// Called by C++ when Zorvax starts moving right — implement in Blueprint to play the flipbook forward
	UFUNCTION(BlueprintImplementableEvent, Category = "Zorvax Animation")
	void PlayRight();

	// Called by C++ when Zorvax starts moving left — implement in Blueprint to play the flipbook in reverse
	UFUNCTION(BlueprintImplementableEvent, Category = "Zorvax Animation")
	void PlayLeft();

	// Current horizontal velocity of Zorvax in units/sec — positive = right, negative = left
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zorvax Movement")
	float XVelocity = 0.0f;

	// Returns the current horizontal velocity — safe to call from Blueprint for debugging or gameplay logic
	UFUNCTION(BlueprintCallable, Category = "Zorvax Movement")
	float GetXVelocity() const { return XVelocity; }

private:
	// Cached reference to the player pawn
	UPROPERTY()
	APawn* PlayerPawn = nullptr;

	// Owner's X position from the previous tick, used to calculate X velocity
	float PreviousX = 0.0f;

	void TickTrackHorizontally(float DeltaTime);
	void UpdateAnimationDirection(float InXVelocity);
	void UpdateXVelocity(float DeltaTime);
};
