// Fill out your copyright notice in the Description page of Project Settings.

#include "PikminThrowTargetComponent.h"
#include "Components/DecalComponent.h"
#include "PikminPlayerController.h"
#include "GameFramework/Character.h"
#include "GameFramework/InputDeviceSubsystem.h"
#include "Engine/Engine.h"
#include "Materials/MaterialInterface.h"

UPikminThrowTargetComponent::UPikminThrowTargetComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPikminThrowTargetComponent::BeginPlay()
{
	Super::BeginPlay();

	if (APawn* OwningPawn = Cast<APawn>(GetOwner()))
	{
		PC = Cast<APikminPlayerController>(OwningPawn->GetController());
	}

	TargetDecal = NewObject<UDecalComponent>(GetOwner(), TEXT("ThrowTargetDecal"));
	TargetDecal->SetupAttachment(GetOwner()->GetRootComponent());
	TargetDecal->SetUsingAbsoluteRotation(true);
	TargetDecal->RegisterComponent();

	TargetDecal->DecalSize = FVector(32.0f, 64.0f, 64.0f);
	TargetDecal->SetWorldRotation(FRotator(-90.0f, 0.0f, 0.0f));
	TargetDecal->SetVisibility(true);

	if (TargetDecalMaterial)
	{
		TargetDecal->SetDecalMaterial(TargetDecalMaterial);
	}

	InitializeInputMode();

	UInputDeviceSubsystem* InputDeviceSubsystem = GEngine->GetEngineSubsystem<UInputDeviceSubsystem>();
	if (InputDeviceSubsystem)
	{
		InputDeviceSubsystem->OnInputHardwareDeviceChanged.AddDynamic(this, &UPikminThrowTargetComponent::HandleHardwareDeviceChanged);
	}
}

void UPikminThrowTargetComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UInputDeviceSubsystem* InputDeviceSubsystem = GEngine->GetEngineSubsystem<UInputDeviceSubsystem>();
	if (InputDeviceSubsystem)
	{
		InputDeviceSubsystem->OnInputHardwareDeviceChanged.RemoveDynamic(this, &UPikminThrowTargetComponent::HandleHardwareDeviceChanged);
	}

	Super::EndPlay(EndPlayReason);
}

void UPikminThrowTargetComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!GetOwner())
	{
		return;
	}

	switch (InputMode)
	{
	case EPikminTargetInputMode::Mouse:
		UpdateTargetLocation_Mouse();
		break;

	default:
		UpdateTargetLocation_Gamepad();
		break;
	}

	TargetDecal->SetWorldLocation(TargetLocation + FVector(0, 0, 2.0f));
}

void UPikminThrowTargetComponent::UpdateTargetLocation_Gamepad()
{
	if (!PC)
	{
		return;
	}

	const ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (!OwnerCharacter)
	{
		return;
	}

	const FRotator YawRot(0.0f, OwnerCharacter->GetActorRotation().Yaw, 0.0f);
	const FVector Forward = YawRot.Vector();

	FVector Desired = OwnerCharacter->GetActorLocation() + Forward * GamepadDistance;

	FVector Hit;
	if (TraceToGround(Desired + FVector(0, 0, TraceHeight), Desired - FVector(0, 0, TraceHeight), Hit))
	{
		TargetLocation = Hit;
	}
}

void UPikminThrowTargetComponent::UpdateTargetLocation_Mouse()
{
	if (!PC)
	{
		return;
	}

	FHitResult Hit;
	if (PC->GetHitResultUnderCursor(ECC_Visibility, false, Hit))
	{
		TargetLocation = Hit.ImpactPoint;
	}
}

bool UPikminThrowTargetComponent::TraceToGround(const FVector& Start, const FVector& End, FVector& OutHit) const
{
	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());

	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
	{
		OutHit = Hit.ImpactPoint;
		return true;
	}

	return false;
}

void UPikminThrowTargetComponent::HandleHardwareDeviceChanged(const FPlatformUserId UserId, const FInputDeviceId DeviceId)
{
	if (UserId != GetOwningPlatformUserId())
	{
		return;
	}

	bool bIsGamepadDevice = false;

	if (UInputDeviceSubsystem* InputDeviceSubsystem = GEngine->GetEngineSubsystem<UInputDeviceSubsystem>())
	{
		const FPlatformUserId OwningUserId = GetOwningPlatformUserId();

		if (OwningUserId.IsValid())
		{
			FHardwareDeviceIdentifier Device = InputDeviceSubsystem->GetMostRecentlyUsedHardwareDevice(OwningUserId);

			if (Device.IsValid())
			{
				bIsGamepadDevice = Device.PrimaryDeviceType == EHardwareDevicePrimaryType::Gamepad;
			}
		}
	}

	InputMode = bIsGamepadDevice ? EPikminTargetInputMode::Gamepad : EPikminTargetInputMode::Mouse;
}

void UPikminThrowTargetComponent::InitializeInputMode()
{
	bool bIsGamepadDevice = false;

	if (UInputDeviceSubsystem* InputDeviceSubsystem = GEngine->GetEngineSubsystem<UInputDeviceSubsystem>())
	{
		const FPlatformUserId OwningUserId = GetOwningPlatformUserId();

		if (OwningUserId.IsValid())
		{
			FHardwareDeviceIdentifier Device = InputDeviceSubsystem->GetMostRecentlyUsedHardwareDevice(OwningUserId);

			if (Device.IsValid())
			{
				bIsGamepadDevice = Device.PrimaryDeviceType == EHardwareDevicePrimaryType::Gamepad;
			}
		}
	}

	InputMode = bIsGamepadDevice ? EPikminTargetInputMode::Gamepad : EPikminTargetInputMode::Mouse;
}

FPlatformUserId UPikminThrowTargetComponent::GetOwningPlatformUserId() const
{
	if (PC)
	{
		if (ULocalPlayer* LocalPlayer = PC->GetLocalPlayer())
		{
			return LocalPlayer->GetPlatformUserId();
		}
	}

	return FPlatformUserId();
}
