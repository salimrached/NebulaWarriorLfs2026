// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_MouseSim.generated.h"

UCLASS()
class NEBULAWARRIOR_API UWidget_MouseSim : public UUserWidget
{
	GENERATED_BODY()

public:

	// Call this when Gamepad Face Button Bottom (A/Cross) is pressed
	UFUNCTION(BlueprintCallable, Category = "Virtual Mouse")
	void SimulateClick();
};
