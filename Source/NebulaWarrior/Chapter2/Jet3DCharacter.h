// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Jet3DCharacter.generated.h"

struct FInputActionValue;
class USplineComponent;

UCLASS()
class NEBULAWARRIOR_API AJet3DCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AJet3DCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;

public:	
	//StaticMeshComponent
	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	class UStaticMeshComponent* JetStaticMesh;
	//CameraComponent
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	class UCameraComponent* CameraComponent;
	//SpringArmComponent
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	class USpringArmComponent* SpringArmComponent;
	//SplinePathComponent
	UPROPERTY(EditAnywhere, Category = "Path")
	class USplineComponent* SplinePathComponent;

	//Input Actions
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* IA_Boost;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* IA_Fire_Primary;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* IA_Fire_PrimaryLock;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* IA_Fire_Secondary;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* IA_Look;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* IA_MoveForward;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* IA_MoveRight;

	//Input Mapping Context
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputMappingContext* IMC_Jet;

	//Input Functions
	void Boost();
	void FirePrimary();
	void FirePrimaryLock();
	void FireSecondary();
	void Look(const FInputActionValue& Value);
	void MoveForward(const FInputActionValue& Value);
	void MoveRight(const FInputActionValue& Value);

	//BlueprintCallable Functions mirroring Input Functions for use in Blueprints
	UFUNCTION(BlueprintImplementableEvent, Category = "Input")
	void Boost_BP();

	UFUNCTION(BlueprintImplementableEvent, Category = "Input")
	void FirePrimary_BP();

	UFUNCTION(BlueprintImplementableEvent, Category = "Input")
	void FirePrimaryLock_BP();

	UFUNCTION(BlueprintImplementableEvent, Category = "Input")
	void FireSecondary_BP();

	UFUNCTION(BlueprintImplementableEvent, Category = "Input")
	void Look_BP(const FInputActionValue& Value);

	UFUNCTION(BlueprintImplementableEvent, Category = "Input")
	void MoveForward_BP(const FInputActionValue& Value);

	UFUNCTION(BlueprintImplementableEvent, Category = "Input")
	void MoveRight_BP(const FInputActionValue& Value);

	//Movement-Related Functions
	//Receives a spline actor and its spline component from Blueprint and caches the spline for path movement
	UFUNCTION(BlueprintCallable, Category = "Path")
	void InitializeSplinePath(AActor* PathActor, USplineComponent* InSplineComponent);
	void MoveAlongSpline(float DeltaTime);

	// How fast the jet travels along the spline (units/sec)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Path")
	float SplineSpeed = 1500.0f;

private:
	// Current distance travelled along the spline
	float SplineDistance = 0.0f;

	// Spline rotation from the previous tick — used to isolate input-driven rotation delta
	FRotator PreviousSplineRotation = FRotator::ZeroRotator;

	// Accumulated input offset in the spline's local space (Up/Right axes), applied on top of spline position
	FVector2D InputOffset = FVector2D::ZeroVector;

};