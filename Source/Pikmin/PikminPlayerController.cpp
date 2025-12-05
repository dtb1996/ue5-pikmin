// Copyright Epic Games, Inc. All Rights Reserved.


#include "PikminPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "Camera/CameraRig.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "PikminPlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Systems/PikminWhistleComponent.h"

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

	//if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	//{
	//	if (InputContext)
	//	{
	//		Subsystem->AddMappingContext(InputContext, 0);
	//	}
	//}

	CameraRig = FindCameraRig();
	if (!CameraRig)
	{
		// try to get camera rig attached to pawn (if pawn spawned rig)
		if (APawn* P = GetPawn())
		{
			TArray<AActor*> ActorChildren;
			P->GetAttachedActors(ActorChildren);
			for (AActor* A : ActorChildren)
			{
				if (ACameraRig* CR = Cast<ACameraRig>(A))
				{
					CameraRig = CR;
					break;
				}
			}
		}
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

	if (!CameraRig)
	{
		CameraRig = FindCameraRig();
	}
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
	//if (APikminPlayerCharacter* Player = Cast<APikminPlayerCharacter>(GetPawn()))
	//{
	//	if ()
	//}

	float Axis = Value.Get<float>();
	FRotator CurrentRotation = GetControlRotation();
	SetControlRotation(CurrentRotation + FRotator(CurrentRotation.Pitch, Axis * 2.5f, CurrentRotation.Roll));

	if (!CameraRig)
	{
		return;
	}

	//float Axis = Value.Get<float>();
	//CameraRig->EnterState(EPikminCameraState::Orbit, 0.15f);
	//// Drive orbit yaw directly (simple)
	//CameraRig->OrbitYaw += Axis * 2.5f;
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

	if (!CameraRig)
	{
		return;
	}

	//float Axis = Value.Get<float>();
	//float NewZoom = FMath::Clamp(CameraRig->ZoomAlpha + Axis * 0.1f, 0.0f, 1.0f);
	//CameraRig->SetZoomAlpha(NewZoom);
}

void APikminPlayerController::HandleWhistleStarted(const FInputActionValue& Value)
{
	if (APawn* PlayerPawn = GetPawn())
	{
		if (auto Whistle = PlayerPawn->FindComponentByClass<UPikminWhistleComponent>())
		{
			Whistle->StartWhistle();
		}
	}

	if (!CameraRig)
	{
		return;
	}

	CameraRig->EnterState(EPikminCameraState::CommandAim, 0.15f);

	// TODO: trigger whistle gameplay effect here
}

void APikminPlayerController::HandleWhistleCompleted(const FInputActionValue& Value)
{
	if (APawn* PlayerPawn = GetPawn())
	{
		if (auto Whistle = PlayerPawn->FindComponentByClass<UPikminWhistleComponent>())
		{
			Whistle->EndWhistle();
		}
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
	if (!CameraRig)
	{
		return;
	}

	CameraRig->EnterState(EPikminCameraState::ThrowAim, 0.10f);
}

void APikminPlayerController::HandleThrowAimReleased(const FInputActionValue& Value)
{
	if (!CameraRig)
	{
		return;
	}

	CameraRig->ExitState(EPikminCameraState::ThrowAim, 0.12f);
}

ACameraRig* APikminPlayerController::FindCameraRig()
{
	// Prefer camera rig attached to pawn
	if (APawn* P = GetPawn())
	{
		TArray<AActor*> Attached;
		P->GetAttachedActors(Attached);
		for (AActor* A : Attached)
		{
			if (ACameraRig* Rig = Cast<ACameraRig>(A))
			{
				return Rig;
			}
		}
	}

	// Otherwise find first placed CameraRig in level
	TArray<AActor*> Found;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACameraRig::StaticClass(), Found);
	if (Found.Num() > 0)
	{
		return Cast<ACameraRig>(Found[0]);
	}

	return nullptr;
}
