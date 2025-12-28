// Copyright Epic Games, Inc. All Rights Reserved.


#include "PikminPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "PikminPlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Systems/PikminWhistleComponent.h"
#include "Systems/PikminThrowTargetComponent.h"

APikminPlayerController::APikminPlayerController()
{
	//bShowMouseCursor = true;
	bReplicates = false;
	//PrimaryActorTick.bCanEverTick = false;
	//bAutoManageActiveCameraTarget = false;
}

void APikminPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalPlayerController())
	{
		FRotator StartRot = GetControlRotation();
		StartRot.Pitch = -30.0f;
		SetControlRotation(StartRot);

		// clear any autopitch from toggling before play
		PitchToggleStartRotation = StartRot;
	}
}

void APikminPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (IsLocalPlayerController())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			if (InputContext)
			{
				Subsystem->AddMappingContext(InputContext, 0);
			}
		}

		if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent))
		{
			EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APikminPlayerController::HandleMoveInput);
			EIC->BindAction(CameraRotateAction, ETriggerEvent::Triggered, this, &APikminPlayerController::HandleCameraRotate);
			EIC->BindAction(CameraZoomAction, ETriggerEvent::Triggered, this, &APikminPlayerController::HandleCameraZoom);
			EIC->BindAction(ResetCameraAction, ETriggerEvent::Triggered, this, &APikminPlayerController::HandleResetCamera);
			EIC->BindAction(CameraTogglePitchAction, ETriggerEvent::Triggered, this, &APikminPlayerController::HandleToggleCameraPitch);
			EIC->BindAction(WhistleAction, ETriggerEvent::Started, this, &APikminPlayerController::HandleWhistleStarted);
			EIC->BindAction(WhistleAction, ETriggerEvent::Completed, this, &APikminPlayerController::HandleWhistleCompleted);
			EIC->BindAction(DismissAction, ETriggerEvent::Started, this, &APikminPlayerController::HandleDismiss);
			EIC->BindAction(ThrowAimAction, ETriggerEvent::Started, this, &APikminPlayerController::HandleThrowAimPressed);
			EIC->BindAction(ThrowAimAction, ETriggerEvent::Completed, this, &APikminPlayerController::HandleThrowAimReleased);
		}
	}
}

void APikminPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!IsLocalPlayerController())
	{
		return;
	}

	// ---- Smooth camera reset ----
	if (bIsResettingCamera)
	{
		ResetElapsed += DeltaSeconds;
		float Alpha = FMath::Clamp(ResetElapsed / ResetDuration, 0.0f, 1.0f);

		// Smooth interpolation
		FRotator NewRotation = FMath::RInterpTo(
			ResetStartRotation,
			ResetTargetRotation,
			ResetElapsed,
			6.5f // smoothing speed
		);

		SetControlRotation(NewRotation);

		if (Alpha >= 1.0f)
		{
			bIsResettingCamera = false;
		}
	}

	// --- Camera pitch toggling ---
	if (bIsTogglingPitch)
	{
		PitchToggleElapsed += DeltaSeconds;
		float Alpha = FMath::Clamp(PitchToggleElapsed / PitchToggleDuration, 0.0f, 1.0f);

		FRotator NewRot = FMath::RInterpTo(
			PitchToggleStartRotation,
			PitchToggleTargetRotation,
			PitchToggleElapsed,
			6.5f  // smoothing speed
		);

		SetControlRotation(NewRot);

		if (Alpha >= 1.0f)
		{
			bIsTogglingPitch = false;
		}
	}

	HandleMouseEdgeCameraRotate(DeltaSeconds);

	CachedMouseXInput = 0.0f;
}

void APikminPlayerController::HandleMoveInput(const FInputActionValue& Value)
{
	FVector2D Axis = Value.Get<FVector2D>();
	if (APawn* P = GetPawn())
	{
		const FRotator YawRot(0, GetControlRotation().Yaw, 0);
		const FVector Forward = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
		const FVector Right = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);

		P->AddMovementInput(Forward, Axis.Y);
		P->AddMovementInput(Right, Axis.X);
	}
}

void APikminPlayerController::HandleCameraRotate(const FInputActionValue& Value)
{
	float Axis = Value.Get<float>();

	// Cache mouse X for edge scrolling
	CachedMouseXInput = Axis;

	// While aiming throw, do not rotate directly
	if (bEnableMouseEdgeRotate && bIsThrowAiming)
	{
		APikminPlayerCharacter* PlayerCharacter = Cast<APikminPlayerCharacter>(GetPawn());
		if (PlayerCharacter && PlayerCharacter->ThrowTargetComponent)
		{
			if (PlayerCharacter->ThrowTargetComponent->GetInputMode() == EPikminTargetInputMode::Mouse)
			{
				return;
			}
		}
	}

	// Normal camera rotation (non-throw)
	FRotator CurrentRotation = GetControlRotation();
	CurrentRotation.Yaw += Axis * 2.5f;
	CurrentRotation.Roll = 0.0f;

	SetControlRotation(CurrentRotation);
}

void APikminPlayerController::HandleCameraZoom(const FInputActionValue& Value)
{
	if (APikminPlayerCharacter* PlayerCharacter = Cast<APikminPlayerCharacter>(GetPawn()))
	{
		if (USpringArmComponent* CameraBoom = PlayerCharacter->CameraBoom)
		{
			float Axis = Value.Get<float>();
			float NewZoom = FMath::Clamp(CameraBoom->TargetArmLength + Axis * 10.0f, 450.0f, 800.0f);
			CameraBoom->TargetArmLength = NewZoom;
		}
	}
}

void APikminPlayerController::HandleResetCamera(const FInputActionValue& Value)
{
	float CurrentTime = GetWorld()->GetTimeSeconds();

	// Check if still in cooldown from previous reset
	if (CurrentTime - LastResetTime < ResetCooldown)
	{
		return;
	}

	LastResetTime = CurrentTime;

	if (APawn* PlayerPawn = GetPawn())
	{
		// target is player's facing direction
		const float DesiredPitch = bOverheadMode ? -60.0f : -30.0f;

		ResetTargetRotation = PlayerPawn->GetActorRotation();
		ResetTargetRotation.Pitch = DesiredPitch;

		// start from current rotation
		ResetStartRotation = GetControlRotation();
		ResetElapsed = 0.0f;

		bIsResettingCamera = true;
	}
}

void APikminPlayerController::HandleToggleCameraPitch(const FInputActionValue& Value)
{
	float CurrentTime = GetWorld()->GetTimeSeconds();

	// Cooldown
	if (CurrentTime - LastPitchToggleTime < PitchToggleCooldown)
	{
		return;
	}

	LastPitchToggleTime = CurrentTime;

	// Determine new mode
	bOverheadMode = !bOverheadMode;

	// Build target pitch
	const float TargetPitch = bOverheadMode ? -60.0f : -30.0f;

	// Set up interpolation
	PitchToggleStartRotation = GetControlRotation();

	PitchToggleTargetRotation = PitchToggleStartRotation;
	PitchToggleTargetRotation.Pitch = TargetPitch;

	PitchToggleElapsed = 0.0f;
	bIsTogglingPitch = true;
}

void APikminPlayerController::HandleWhistleStarted(const FInputActionValue& Value)
{
	if (APikminPlayerCharacter* PlayerCharacter = Cast<APikminPlayerCharacter>(GetPawn()))
	{
		PlayerCharacter->StartWhistle();
	}

	// TODO: trigger whistle gameplay effect here
}

void APikminPlayerController::HandleWhistleCompleted(const FInputActionValue& Value)
{
	if (APikminPlayerCharacter* PlayerCharacter = Cast<APikminPlayerCharacter>(GetPawn()))
	{
		PlayerCharacter->EndWhistle();
	}
}

void APikminPlayerController::HandleDismiss(const FInputActionValue& Value)
{
	if (APikminPlayerCharacter* PlayerCharacter = Cast<APikminPlayerCharacter>(GetPawn()))
	{
		PlayerCharacter->CommandDismiss();
	}
}

void APikminPlayerController::HandleThrowAimPressed(const FInputActionValue& Value)
{
	bIsThrowAiming = true;

	if (APikminPlayerCharacter* PlayerCharacter = Cast<APikminPlayerCharacter>(GetPawn()))
	{
		PlayerCharacter->CommandAim();
		PlayerCharacter->SetMovementEnabled(false);
	}
}

void APikminPlayerController::HandleThrowAimReleased(const FInputActionValue& Value)
{
	bIsThrowAiming = false
		;
	if (APikminPlayerCharacter* PlayerCharacter = Cast<APikminPlayerCharacter>(GetPawn()))
	{
		PlayerCharacter->CommandThrow();
		PlayerCharacter->SetMovementEnabled(true);
	}
}

void APikminPlayerController::HandleMouseEdgeCameraRotate(float DeltaSeconds)
{
	if (!bEnableMouseEdgeRotate)
	{
		return;
	}

	if (!bIsThrowAiming)
	{
		return;
	}

	// Require actual mouse X input this frame
	if (FMath::IsNearlyZero(CachedMouseXInput))
	{
		return;
	}

	APikminPlayerCharacter* PlayerCharacter = Cast<APikminPlayerCharacter>(GetPawn());
	if (!PlayerCharacter || !PlayerCharacter->ThrowTargetComponent)
	{
		return;
	}

	// Only when using mouse targeting
	if (PlayerCharacter->ThrowTargetComponent->GetInputMode() != EPikminTargetInputMode::Mouse)
	{
		return;
	}

	FVector2D MousePos;
	if (!GetMousePosition(MousePos.X, MousePos.Y))
	{
		return;
	}

	float ZoomScale = 1.0f;

	if (PlayerCharacter->CameraBoom)
	{
		const float ArmLength = PlayerCharacter->CameraBoom->TargetArmLength;

		// Match your zoom limits
		const float MinZoom = 450.0f;
		const float MaxZoom = 800.0f;

		// 0 = close, 1 = far
		float ZoomAlpha = FMath::Clamp((ArmLength - MinZoom) / (MaxZoom - MinZoom), 0.0f, 1.0f);

		// Slight bias so close zoom isn’t *too* slow
		ZoomScale = FMath::Lerp(0.6f, 1.25f, ZoomAlpha);
	}

	int32 ViewX, ViewY;
	GetViewportSize(ViewX, ViewY);

	float YawDelta = 0.0f;

	// Left edge
	if (MousePos.X <= EdgeRotateMarginPx && CachedMouseXInput < 0.0f)
	{
		float Alpha = 1.0f - (MousePos.X / EdgeRotateMarginPx);
		YawDelta = -MaxEdgeRotateSpeed * Alpha;
	}
	// Right edge
	else if (MousePos.X >= ViewX - EdgeRotateMarginPx && CachedMouseXInput > 0.0f)
	{
		float Alpha = (MousePos.X - (ViewX - EdgeRotateMarginPx)) / EdgeRotateMarginPx;
		YawDelta = MaxEdgeRotateSpeed * Alpha;
	}

	if (!FMath::IsNearlyZero(YawDelta))
	{
		FRotator NewRot = GetControlRotation();
		NewRot.Yaw += YawDelta * ZoomScale * DeltaSeconds;
		SetControlRotation(NewRot);
	}
}
