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
	// Controller rotation drives yaw and pitch — spline feeds deltas, Look input stacks on top
	bUseControllerRotationYaw   = true;
	bUseControllerRotationPitch = true;
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
	//MoveAlongSpline(DeltaTime);
}

// SetupPlayerInputComponent
void AJet3DCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Boost
		if (IA_Boost)
			EIC->BindAction(IA_Boost, ETriggerEvent::Triggered, this, &AJet3DCharacter::Boost);

		// Fire Primary
		if (IA_Fire_Primary)
			EIC->BindAction(IA_Fire_Primary, ETriggerEvent::Triggered, this, &AJet3DCharacter::FirePrimary);

		// Fire Primary Lock
		if (IA_Fire_PrimaryLock)
			EIC->BindAction(IA_Fire_PrimaryLock, ETriggerEvent::Started, this, &AJet3DCharacter::FirePrimaryLock);

		// Fire Secondary
		if (IA_Fire_Secondary)
			EIC->BindAction(IA_Fire_Secondary, ETriggerEvent::Started, this, &AJet3DCharacter::FireSecondary);

		// Look
		if (IA_Look)
			EIC->BindAction(IA_Look, ETriggerEvent::Triggered, this, &AJet3DCharacter::Look);

		// Move Up
		if (IA_MoveForward)
			EIC->BindAction(IA_MoveForward, ETriggerEvent::Triggered, this, &AJet3DCharacter::MoveUp);

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
	//AddControllerYawInput(LookAxis.X);
	//AddControllerPitchInput(-LookAxis.Y);
	Look_BP(LookAxis);
}

// MoveUp
void AJet3DCharacter::MoveUp(const FInputActionValue& Value)
{
	MoveUp_BP(Value);
}

// MoveRight
void AJet3DCharacter::MoveRight(const FInputActionValue& Value)
{
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

	// Advance distance along the spline (mirrors Blueprint: SplineDistance + SplineSpeed * DeltaSeconds)
	const float SplineLength = SplinePathComponent->GetSplineLength();
	SplineDistance = FMath::Fmod(SplineDistance + SplineSpeed * DeltaTime, SplineLength);

	// Set location (mirrors Blueprint: GetLocationAtDistanceAlongSpline → SetActorLocation)
	const FVector NewLocation = SplinePathComponent->GetLocationAtDistanceAlongSpline(SplineDistance, ESplineCoordinateSpace::World);
	SetActorLocation(NewLocation);

	// Set rotation (mirrors Blueprint: GetRotationAtDistanceAlongSpline → SetActorRotation)
	const FRotator NewRotation = SplinePathComponent->GetRotationAtDistanceAlongSpline(SplineDistance, ESplineCoordinateSpace::World);
	SetActorRotation(NewRotation);
}


