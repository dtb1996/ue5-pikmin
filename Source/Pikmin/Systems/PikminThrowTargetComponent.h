// Fill out your copyright notice in the Description page of Project Settings.

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
	FVector GetTargetLocation() const { return TargetLocation; };

	UFUNCTION(BlueprintCallable)
	EPikminTargetInputMode GetInputMode() const { return InputMode; };

private:
	void UpdateTargetLocation_Gamepad();
	void UpdateTargetLocation_Mouse();
	bool TraceToGround(const FVector& Start, const FVector& End, FVector& OutHit) const;

	UFUNCTION()
	void HandleHardwareDeviceChanged(const FPlatformUserId UserId, const FInputDeviceId DeviceId);
	
	void InitializeInputMode();
	FPlatformUserId GetOwningPlatformUserId() const;

private:
	UPROPERTY(EditAnywhere, Category = "Config")
	UMaterialInterface* TargetDecalMaterial = nullptr;
	
	UPROPERTY(Transient)
	UDecalComponent* TargetDecal;

	UPROPERTY(Transient)
	APikminPlayerController* PC;

	FVector TargetLocation;

	EPikminTargetInputMode InputMode = EPikminTargetInputMode::Mouse;

	UPROPERTY(EditAnywhere, Category = "Config")
	float GamepadDistance = 250.0f;

	UPROPERTY(EditAnywhere, Category = "Config")
	float TraceHeight = 1500.0f;
};
