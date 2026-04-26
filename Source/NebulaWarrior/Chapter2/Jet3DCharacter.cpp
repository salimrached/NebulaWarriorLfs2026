// Fill out your copyright notice in the Description page of Project Settings.


#include "Jet3DCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SplineComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

// Constructor
AJet3DCharacter::AJet3DCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	// Set default movement mode to Flying
	GetCharacterMovement()->DefaultLandMovementMode = MOVE_Flying;
	// Disable CMC's own position updates — spline drives location directly
	GetCharacterMovement()->SetMovementMode(MOVE_None);
	// Allow the controller rotation to drive all three axes on the actor
	bUseControllerRotationYaw   = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll  = false;
	//StaticMeshComponent
	JetStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("JetStaticMesh"));
	JetStaticMesh->SetupAttachment(RootComponent);
	//SpringArmComponent
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->TargetArmLength = 600.0f;
	//CameraComponent
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent);
}

// BeginPlay
void AJet3DCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Add Input Mapping Context
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			if (IMC_Jet)
			{
				Subsystem->AddMappingContext(IMC_Jet, 0);
			}
		}
	}
}

// Tick
void AJet3DCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	MoveAlongSpline(DeltaTime);
}

// SetupPlayerInputComponent
void AJet3DCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Boost
		if (IA_Boost)
			EIC->BindAction(IA_Boost, ETriggerEvent::Started, this, &AJet3DCharacter::Boost);

		// Fire Primary
		if (IA_Fire_Primary)
			EIC->BindAction(IA_Fire_Primary, ETriggerEvent::Triggered, this, &AJet3DCharacter::FirePrimary);

		// Fire Primary Lock
		if (IA_Fire_PrimaryLock)
			EIC->BindAction(IA_Fire_PrimaryLock, ETriggerEvent::Started, this, &AJet3DCharacter::FirePrimaryLock);

		// Fire Secondary
		if (IA_Fire_Secondary)
			EIC->BindAction(IA_Fire_Secondary, ETriggerEvent::Triggered, this, &AJet3DCharacter::FireSecondary);

		// Look
		if (IA_Look)
			EIC->BindAction(IA_Look, ETriggerEvent::Triggered, this, &AJet3DCharacter::Look);

		// Move Forward
		if (IA_MoveForward)
			EIC->BindAction(IA_MoveForward, ETriggerEvent::Triggered, this, &AJet3DCharacter::MoveForward);

		// Move Right
		if (IA_MoveRight)
			EIC->BindAction(IA_MoveRight, ETriggerEvent::Triggered, this, &AJet3DCharacter::MoveRight);
	}
}

// Boost
void AJet3DCharacter::Boost()
{
	Boost_BP();
}

// FirePrimary
void AJet3DCharacter::FirePrimary()
{
	FirePrimary_BP();
}

// FirePrimaryLock
void AJet3DCharacter::FirePrimaryLock()
{
	FirePrimaryLock_BP();
}

// FireSecondary
void AJet3DCharacter::FireSecondary()
{
	FireSecondary_BP();
}

// Look
void AJet3DCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxis = Value.Get<FVector2D>();
	AddControllerYawInput(LookAxis.X);
	AddControllerPitchInput(LookAxis.Y);
	Look_BP(LookAxis);
}

// MoveForward
void AJet3DCharacter::MoveForward(const FInputActionValue& Value)
{
	const float AxisValue = Value.Get<float>();
	InputOffset.Y += AxisValue;
	MoveForward_BP(Value);
}

// MoveRight
void AJet3DCharacter::MoveRight(const FInputActionValue& Value)
{
	const float AxisValue = Value.Get<float>();
	InputOffset.X += AxisValue;
	MoveRight_BP(Value);
}

void AJet3DCharacter::InitializeSplinePath(AActor* PathActor, USplineComponent* InSplineComponent)
{
	if (!InSplineComponent)
	{
		return;
	}

	SplinePathComponent = InSplineComponent;
	SplineDistance = 0.0f;
	PreviousSplineRotation = SplinePathComponent->GetRotationAtDistanceAlongSpline(0.0f, ESplineCoordinateSpace::World);
}

// MoveAlongSpline
void AJet3DCharacter::MoveAlongSpline(float DeltaTime)
{
	if (!SplinePathComponent)
	{
		return;
	}

	// Advance distance along the spline
	const float SplineLength = SplinePathComponent->GetSplineLength();
	SplineDistance = FMath::Fmod(SplineDistance + SplineSpeed * DeltaTime, SplineLength);

	// Get the spline's world-space axes at the current distance
	const FVector SplineLocation = SplinePathComponent->GetLocationAtDistanceAlongSpline(SplineDistance, ESplineCoordinateSpace::World);
	const FVector SplineRight    = SplinePathComponent->GetRightVectorAtDistanceAlongSpline(SplineDistance, ESplineCoordinateSpace::World);
	const FVector SplineUp       = SplinePathComponent->GetUpVectorAtDistanceAlongSpline(SplineDistance, ESplineCoordinateSpace::World);

	// Snap actor to spline position + accumulated input offset along the spline's own Right and Up axes
	const FVector FinalLocation = SplineLocation
		+ SplineRight * InputOffset.X
		+ SplineUp    * InputOffset.Y;

	SetActorLocation(FinalLocation, true);

	// Rotation: snap to spline Yaw and Pitch, leave Roll at zero
	const FRotator CurrentSplineRotation = SplinePathComponent->GetRotationAtDistanceAlongSpline(SplineDistance, ESplineCoordinateSpace::World);
	SetActorRotation(FRotator(CurrentSplineRotation.Pitch, CurrentSplineRotation.Yaw, 0.0f));
	PreviousSplineRotation = CurrentSplineRotation;
}


