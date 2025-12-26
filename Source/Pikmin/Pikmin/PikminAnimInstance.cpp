// Copyright 2025, Rolling Pixels. All Rights Reserved


#include "PikminAnimInstance.h"
#include "Pikmin/PikminCharacter.h"

void UPikminAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	PikminOwner = Cast<APikminCharacter>(TryGetPawnOwner());
}

void UPikminAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!PikminOwner)
	{
		PikminOwner = Cast<APikminCharacter>(TryGetPawnOwner());
		return;
	}

	Speed = PikminOwner->GetVelocity().Size2D();

	// State
	EPikminState State = PikminOwner->GetState();

	bIsCarrying = (State == EPikminState::Working);
	bIsThrown = (State == EPikminState::Thrown);

	// Animation speed source
	if (bIsCarrying)
	{
		CarryAnimSpeed = PikminOwner->CarryAnimSpeed;
	}
	else
	{
		CarryAnimSpeed = Speed;
	}
}
