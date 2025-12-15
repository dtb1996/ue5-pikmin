// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PikminPlayerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class PIKMIN_API UPikminPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly, Category="Pikmin")
	bool bIsWhistling = false;

	UPROPERTY(BlueprintReadOnly, Category="Pikmin")
	bool bIsThrowing = false;

	UFUNCTION(BlueprintCallable)
	void PlayThrowMontage();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pikmin")
	UAnimMontage* ThrowMontage;
};
