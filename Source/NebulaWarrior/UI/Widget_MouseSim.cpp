// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget_MouseSim.h"
#include "Framework/Application/SlateApplication.h"

void UWidget_MouseSim::SimulateClick()
{
	if (!FSlateApplication::IsInitialized())
	{
		return;
	}

	const FVector2D CursorPos = FSlateApplication::Get().GetCursorPos();

	// Simulate left mouse button press
	FPointerEvent MouseDown(
		0,
		CursorPos,
		CursorPos,
		TSet<FKey>(),
		EKeys::LeftMouseButton,
		0.f,
		FModifierKeysState()
	);
	FSlateApplication::Get().ProcessMouseButtonDownEvent(nullptr, MouseDown);

	// Simulate left mouse button release
	FPointerEvent MouseUp(
		0,
		CursorPos,
		CursorPos,
		TSet<FKey>(),
		EKeys::LeftMouseButton,
		0.f,
		FModifierKeysState()
	);
	FSlateApplication::Get().ProcessMouseButtonUpEvent(MouseUp);
}

