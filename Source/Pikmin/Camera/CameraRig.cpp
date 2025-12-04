// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraRig.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraShakeBase.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

ACameraRig::ACameraRig()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = false;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SetRootComponent(SpringArm);
	SpringArm->TargetArmLength = 600.f;
	SpringArm->bUsePawnControlRotation = false;
	SpringArm->bEnableCameraLag = false;
	SpringArm->bDoCollisionTest = true;
	SpringArm->ProbeSize = 12.f;
	SpringArm->bEnableCameraRotationLag = false;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;

	CurrentState = EPikminCameraState::Idle;
	StateBlendAlpha = 1.f;
	StateBlendSpeed = 10.f;
	ZoomAlpha = 0.f;
	OrbitYaw = 0.f;

	SoftZoneHalfExtents = FVector2D(200.f, 120.f);
	LookAheadDistance = 250.f;
	FollowLerpSpeed = 8.f;
	RotationLerpSpeed = 10.f;
	MaxAutoTilt = -6.f;
	MinArmLength = 420.f;
	MaxArmLength = 900.f;
	ArmLerpSpeed = 6.f;
}

void ACameraRig::BeginPlay()
{
	Super::BeginPlay();
	
	SmoothedLocation = GetActorLocation();
	SmoothedRotation = GetActorRotation();
	SmoothedArmLength = SpringArm->TargetArmLength;
}

void ACameraRig::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector DesiredLocation;
	FRotator DesiredRotation;
	float DesiredArmLength;
	ComputeDesiredTransform(DeltaTime, DesiredLocation, DesiredRotation, DesiredArmLength);

	SmoothedLocation = FMath::VInterpTo(SmoothedLocation, DesiredLocation, DeltaTime, FollowLerpSpeed);
	SmoothedRotation = FMath::RInterpTo(SmoothedRotation, DesiredRotation, DeltaTime, RotationLerpSpeed);
	SmoothedArmLength = FMath::FInterpTo(SmoothedArmLength, DesiredArmLength, DeltaTime, ArmLerpSpeed);

	SetActorLocation(SmoothedLocation);
	SetActorRotation(SmoothedRotation);
	SpringArm->TargetArmLength = SmoothedArmLength;

	// Debug
	if (bDrawDebug)
	{
		DrawDebugSphere(GetWorld(), AimFocusPoint, 12.f, 8, FColor::Yellow);
		DrawDebugBox(GetWorld(), SmoothedLocation, FVector(SoftZoneHalfExtents.X, SoftZoneHalfExtents.Y, 12.f), FQuat(SmoothedRotation), FColor::Cyan);
	}
}

void ACameraRig::EnterState(EPikminCameraState NewState, float BlendTime)
{
	// Enter state with blend speed set by BlendTime
	CurrentState = NewState;
	StateBlendSpeed = (BlendTime > KINDA_SMALL_NUMBER) ? (1.0f / BlendTime) : 10.0f;
	StateBlendAlpha = 0.0f;

	// Example: small shake entering command/throw
	if ((NewState == EPikminCameraState::CommandAim || NewState == EPikminCameraState::ThrowAim) && SmallShake)
	{
		if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
		{
			PC->ClientStartCameraShake(SmallShake);
		}
	}
}

void ACameraRig::ExitState(EPikminCameraState OldState, float BlendTime)
{
	// For prototype, fallback to Idle on exit
	CurrentState = EPikminCameraState::Idle;
	StateBlendSpeed = (BlendTime > KINDA_SMALL_NUMBER) ? (1.0f / BlendTime) : 10.0f;
	StateBlendAlpha = 0.f;
}

void ACameraRig::SetFollowTarget(AActor* NewTarget)
{
	FollowTarget = NewTarget;
}

void ACameraRig::SetFocusPoint(const FVector& WorldPoint)
{
	AimFocusPoint = WorldPoint;
}

void ACameraRig::SetZoomAlpha(float Alpha)
{
	ZoomAlpha = FMath::Clamp(Alpha, 0.0f, 1.0f);
}

void ACameraRig::ComputeDesiredTransform(float DeltaTime, FVector& OutLocation, FRotator& OutRotation, float& OutArmLength)
{
	if (!FollowTarget)
	{
		OutLocation = GetActorLocation();
		OutRotation = GetActorRotation();
		OutArmLength = SpringArm->TargetArmLength;
		return;
	}

	// Base follow location (player location)
	const FVector PlayerLocation = FollowTarget->GetActorLocation();

	// Compute look-ahead using velocity if pawn
	FVector Velocity = FVector::ZeroVector;
	APawn* Pawn = Cast<APawn>(FollowTarget);
	if (Pawn)
	{
		Velocity = Pawn->GetVelocity();
	}

	FVector LookAhead = Velocity.GetSafeNormal() * LookAheadDistance * (Velocity.Size() > KINDA_SMALL_NUMBER ? 1.0f : 0.0f);

	// Compute soft-zone correction
	FVector CameraWorldLocation = GetActorLocation();
	FVector SoftCorrection = ComputeSoftZoneCorrection(PlayerLocation + LookAhead, CameraWorldLocation);

	// Prepare a focus point depending on state
	FVector StateFocus = PlayerLocation;
	FRotator StateRotation = FRotator(-50.0f, 0.0f, 0.0f);

	switch (CurrentState)
	{
	case EPikminCameraState::Idle:
		StateFocus = PlayerLocation + LookAhead;
		break;

	case EPikminCameraState::CommandAim:
	case EPikminCameraState::ThrowAim:
		// Bias camera to look at AimFocusPoint
		{
			FVector Mid = (PlayerLocation + AimFocusPoint) * 0.5f;
			FVector Dir = (AimFocusPoint - PlayerLocation).GetSafeNormal();
			StateFocus = Mid + Dir * (LookAheadDistance * 0.25f);
			StateRotation = UKismetMathLibrary::FindLookAtRotation(StateFocus, AimFocusPoint);
			//Clamp pitch so it's not flat
			StateRotation.Pitch = FMath::Clamp(StateRotation.Pitch, -80.0f, -20.0f);
		}
		break;

	case EPikminCameraState::Zoomed:
		StateFocus = PlayerLocation + LookAhead * 0.5f;
		break;

	case EPikminCameraState::Orbit:
		{
			// Allow orbiting around player by a yaw offset
			StateFocus = PlayerLocation;
			if (APawn* P = Cast<APawn>(FollowTarget))
			{
				// TODO: OrbitYaw can be driven by controller input
				StateRotation = FRotator(StateRotation.Pitch, OrbitYaw, 0.0f);
			}
		}
		break;

	default:
		StateFocus = PlayerLocation + LookAhead;
		break;
	}

	// Apply soft-zone correction
	FVector DesiredCenter = StateFocus + SoftCorrection;

	// Auto tilt based on speed
	float SpeedRatio = FMath::Clamp(Velocity.Size() / 600.0f, 0.0f, 1.0f);
	float TiltPitch = FMath::Lerp(0.0f, MaxAutoTilt, SpeedRatio);

	// Compose desired rotation (we want the camera to look at the center from above)
	FVector CameraDesiredPosition = DesiredCenter + FVector(0.0f, 0.0f, 0.0f); // the actor location will be smoothed
	FRotator LookRotation = UKismetMathLibrary::FindLookAtRotation(DesiredCenter + FVector(0.0f, 0.0f, 400.0f), DesiredCenter);
	LookRotation.Pitch += TiltPitch;

	// Desired actor location is offset behind and above the desired center
	FRotator ActorRotation = FRotator(0.0f, LookRotation.Yaw, 0.0f);
	float TargetArmLength = FMath::Lerp(MinArmLength, MaxArmLength, ZoomAlpha);

	// Get a location behind the desired center according to yaw and arm length
	FVector BackOffset = ActorRotation.RotateVector(FVector(-TargetArmLength * 0.5f, 0.0f, 0.0f));
	FVector DesiredActorLocation = DesiredCenter + BackOffset + FVector(0.0f, 0.0f, TargetArmLength * 0.4f);

	// If springarm collision shortens, let springarm handle it. We still set actor location and rotation.
	OutLocation = DesiredActorLocation;
	OutRotation = LookRotation;
	OutArmLength = TargetArmLength;
}

FVector ACameraRig::ComputeSoftZoneCorrection(const FVector& WorldTargetLocation, const FVector& CameraWorldLocation) const
{
	// Convert world target into camera-local 2D space
	FVector ToTarget = WorldTargetLocation - CameraWorldLocation;
	FVector Forward = GetActorForwardVector();
	FVector Right = GetActorRightVector();

	float X = FVector::DotProduct(ToTarget, Right);
	float Y = FVector::DotProduct(ToTarget, Forward);

	float CorrectionX = 0.0f;
	float CorrectionY = 0.0f;

	if (FMath::Abs(X) > SoftZoneHalfExtents.X)
	{
		CorrectionX = (FMath::Abs(X) - SoftZoneHalfExtents.X) * FMath::Sign(X);
	}
	if (FMath::Abs(Y) > SoftZoneHalfExtents.Y)
	{
		CorrectionX = (FMath::Abs(Y) - SoftZoneHalfExtents.Y) * FMath::Sign(Y);
	}

	return Right * CorrectionX + Forward * CorrectionY;
}

FVector ACameraRig::ResolveCameraCollision(const FVector& DesiredCameraLocation, const FVector& TraceStart) const
{
	// Placeholder - springarm does collision; for custom system you would trace here
	return DesiredCameraLocation;
}

