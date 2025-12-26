// Copyright 2025, Rolling Pixels. All Rights Reserved

#include "PikminPlayerAnimInstance.h"

void UPikminPlayerAnimInstance::PlayThrowMontage()
{
	if (ThrowMontage && !Montage_IsPlaying(ThrowMontage))
	{
		Montage_Play(ThrowMontage);
	}
}
