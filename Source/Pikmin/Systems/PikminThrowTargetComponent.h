// Copyright 2025, Rolling Pixels. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PikminThrowTargetComponent.generated.h"

class UDecalComponent;
class APikminPlayerController;
class UMaterialInterface;

UENUM()
enum class EPikminTargetInputMode : uint8
{
	Gamepad,
	Mouse
};

UENUM()
enum class EPikminMouseTargetState : uint8
{
	Passive,
	Aiming
};


UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent) )
class PIKMIN_API UPikminThrowTargetComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPikminThrowTargetComponent();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	bool HasValidTarget() const { return !TargetLocation.IsNearlyZero(); };

	UFUNCTION(BlueprintCallable)
	FVector GetTargetLocation() const { return TargetLocation; };

	UFUNCTION(BlueprintCallable)
	EPikminTargetInputMode GetInputMode() const { return InputMode; };

	UFUNCTION(BlueprintCallable)
	void BeginMouseAim();

	UFUNCTION(BlueprintCallable)
	void EndMouseAim();

private:
	void UpdateTargetLocation_Gamepad();
	void UpdateTargetLocation_Mouse();
	bool TraceToGround(const FVector& Start, const FVector& End, FVector& OutHit) const;
	FVector ClampToMaxDistance(const FVector& DesiredWorldPos) const;

	UFUNCTION()
	void HandleHardwareDeviceChanged(const FPlatformUserId UserId, const FInputDeviceId DeviceId);
	
	void InitializeInputMode();
	FPlatformUserId GetOwningPlatformUserId() const;

private:
	UPROPERTY(EditAnywhere, Category = "Config|Gamepad")
	float MaxTargetDistance_Gamepad = 500.f;

	UPROPERTY(EditAnywhere, Category = "Config|Mouse")
	float MaxTargetDistance_Mouse = 900.f;

	UPROPERTY(EditAnywhere, Category = "Config")
	UMaterialInterface* TargetDecalMaterial = nullptr;
	
	UPROPERTY(Transient)
	UDecalComponent* TargetDecal;

	UPROPERTY(Transient)
	APikminPlayerController* PC;

	FVector TargetLocation;

	EPikminTargetInputMode InputMode = EPikminTargetInputMode::Mouse;

	EPikminMouseTargetState MouseTargetState = EPikminMouseTargetState::Passive;

	UPROPERTY(EditAnywhere, Category = "Config")
	float GamepadDistance = 250.0f;

	UPROPERTY(EditAnywhere, Category = "Config")
	float TraceHeight = 1500.0f;
};
