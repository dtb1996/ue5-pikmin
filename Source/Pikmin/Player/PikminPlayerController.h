// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "PikminPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;

UCLASS(abstract)
class APikminPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
    APikminPlayerController();
	
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
    virtual void Tick(float DeltaSeconds) override;

private:
    /** Input mapping context */
    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputMappingContext* InputContext;

    /** Input actions */
    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputAction* MoveAction;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputAction* CameraRotateAction;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputAction* CameraZoomAction;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputAction* ResetCameraAction;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputAction* CameraTogglePitchAction;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputAction* WhistleAction;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputAction* DismissAction;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputAction* ThrowAimAction;

    UPROPERTY(EditDefaultsOnly, Category = "Camera|Mouse Edge Rotate")
    float EdgeRotateMarginPx = 128.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Camera|Mouse Edge Rotate")
    float MaxEdgeRotateSpeed = 200.0f; // deg/sec

    UPROPERTY(EditDefaultsOnly, Category = "Camera|Mouse Edge Rotate")
    bool bEnableMouseEdgeRotate = true;

    // --- Camera Reset Smooth Lerp ---
    bool bIsResettingCamera = false;
    FRotator ResetStartRotation;
    FRotator ResetTargetRotation;
    float ResetElapsed = 0.0f;
    float ResetDuration = 0.35f;

    // --- Camera Reset Cooldown ---
    float LastResetTime = -999.0f;
    float ResetCooldown = 0.4f;

    // --- Camera Pitch Toggle ---
    bool bIsTogglingPitch = false;
    bool bOverheadMode = false;

    FRotator PitchToggleStartRotation;
    FRotator PitchToggleTargetRotation;

    float PitchToggleElapsed = 0.0f;
    float PitchToggleDuration = 0.35f;

    // --- Camera mode cooldown ---
    float LastPitchToggleTime = -999.0f;
    float PitchToggleCooldown = 0.4f;

    float CachedMouseXInput = 0.0f;
    bool bIsThrowAiming = false;

private:
    void HandleMoveInput(const FInputActionValue& Value);
    void HandleCameraRotate(const FInputActionValue& Value);
    void HandleCameraZoom(const FInputActionValue& Value);
    void HandleResetCamera(const FInputActionValue& Value);
    void HandleToggleCameraPitch(const FInputActionValue& Value);
    void HandleWhistleStarted(const FInputActionValue& Value);
    void HandleWhistleCompleted(const FInputActionValue& Value);
    void HandleDismiss(const FInputActionValue& Value);
    void HandleThrowAimPressed(const FInputActionValue& Value);
    void HandleThrowAimReleased(const FInputActionValue& Value);

    void HandleMouseEdgeCameraRotate(float DeltaSeconds);
};
