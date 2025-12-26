// Fill out your copyright notice in the Description page of Project Settings.

#include "PikminPlayerAnimInstance.h"

void UPikminPlayerAnimInstance::PlayThrowMontage()
{
	if (ThrowMontage && !Montage_IsPlaying(ThrowMontage))
	{
		Montage_Play(ThrowMontage);
	}
}
