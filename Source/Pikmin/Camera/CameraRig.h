// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Camera/CameraTypes.h"
#include "CameraRig.generated.h"

class USpringArmComponent;
class UCameraComponent;

UENUM(BlueprintType)
enum class EPikminCameraState : uint8
{
	Idle        UMETA(DisplayName = "Idle"),
	CommandAim  UMETA(DisplayName = "CommandAim"),
	ThrowAim    UMETA(DisplayName = "ThrowAim"),
	Zoomed      UMETA(DisplayName = "Zoomed"),
	Orbit       UMETA(DisplayName = "Orbit")
};

UCLASS()
class PIKMIN_API ACameraRig : public AActor
{
	GENERATED_BODY()
	
public:	
	ACameraRig();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	/** Public API for external callers */
	UFUNCTION(BlueprintCallable, Category = "Camera")
	void EnterState(EPikminCameraState NewState, float BlendTime = 0.2f);

	UFUNCTION(BlueprintCallable, Category = "Camera")
	void ExitState(EPikminCameraState OldState, float BlendTime = 0.2f);

	/** Set the follow target (player pawn) */
	UFUNCTION(BlueprintCallable, Category = "Camera")
	void SetFollowTarget(AActor* NewTarget);

	/** Point in world for aim focus (used in Command/Throw states */
	UFUNCTION(BlueprintCallable, Category = "Camera")
	void SetFocusPoint(const FVector& WorldPoint);

	/** Manually change zoom alpha (0 = default, 1 = zoomed) */
	UFUNCTION(BlueprintCallable, Category = "Camera")
	void SetZoomAlpha(float Alpha);

protected:
	/** Camera components */
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	USpringArmComponent* SpringArm;

public:
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	UCameraComponent* Camera;

protected:
	/** Follow target (player pawn) */
	UPROPERTY()
	AActor* FollowTarget;

	/** Current camera state */
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	EPikminCameraState CurrentState;

	/** Blend interpolation alpha for state transitions */
	float StateBlendAlpha;
	float StateBlendSpeed;

	/** Soft zone (world-space box centered at the follow target) */
	UPROPERTY(EditAnywhere, Category = "Camera|Framing")
	FVector2D SoftZoneHalfExtents = FVector2D(200.0f, 120.0f);

	/** Look-ahead distance based on velocity */
	UPROPERTY(EditAnywhere, Category = "Camera|Framing")
	float LookAheadDistance = 250.0f;

	/** Camera lag / smoothing */
	UPROPERTY(EditAnywhere, Category = "Camera|Smoothing")
	float FollowLerpSpeed = 8.0f;

	/** Rotation smoothing */
	UPROPERTY(EditAnywhere, Category = "Camera|Smoothing")
	float RotationLerpSpeed = 10.0f;

	/** Auto tilt (maximum pitch offset when at max speed) */
	UPROPERTY(EditAnywhere, Category = "Camera|Framing")
	float MaxAutoTilt = -6.0f;

	/** Dynamic height range (arm length) */
	UPROPERTY(EditAnywhere, Category = "Camera|Framing")
	float MinArmLength = 420.0f;

	UPROPERTY(EditAnywhere, Category = "Camera|Framing")
	float MaxArmLength = 900.0f;

	/** Arm length lerp speed */
	UPROPERTY(EditAnywhere, Category = "Camera|Framing")
	float ArmLerpSpeed = 6.0f;

	/** Focus point used for aim states */
	FVector AimFocusPoint;

public:
	/** Zoom alpha */
	float ZoomAlpha;

	/** Orbit yaw offset (used in Orbit state) */
	float OrbitYaw;

protected:
	/** Helper: compute desired camera transform for the frame */
	void ComputeDesiredTransform(float DeltaTime, FVector& OutLocation, FRotator& OutRotation, float& OutArmLength);

	/** Soft-zone helper */
	FVector ComputeSoftZoneCorrection(const FVector& WorldTargetLocation, const FVector& CameraWorldLocation) const;

	/** Internal smoothing helpers */
	FVector SmoothedLocation;
	FRotator SmoothedRotation;
	float SmoothedArmLength;

	/** Small helper for collision test fallback (if springarm disabled collision) */
	FVector ResolveCameraCollision(const FVector& DesiredCameraLocation, const FVector& TraceStart) const;

	/** Camera shake */
	UPROPERTY(EditAnywhere, Category = "Camera|Effects")
	TSubclassOf<UCameraShakeBase> SmallShake;

	/** Debug */
	UPROPERTY(EditAnywhere, Category = "Camera|Debug")
	bool bDrawDebug = false;
};
