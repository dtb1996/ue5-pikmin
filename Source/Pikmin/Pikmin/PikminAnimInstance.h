// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PikminAnimInstance.generated.h"

class APikminCharacter;

/**
 * 
 */
UCLASS()
class PIKMIN_API UPikminAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

public:
	UPROPERTY(BlueprintReadOnly, Category = "Pikmin")
	bool bIsCarrying;

	UPROPERTY(BlueprintReadOnly, Category = "Pikmin")
	bool bIsThrown;

	UPROPERTY(BlueprintReadOnly, Category = "Pikmin")
	float Speed;
	
	UPROPERTY(BlueprintReadOnly, Category = "Pikmin")
	float CarryAnimSpeed;

protected:
	APikminCharacter* PikminOwner;
};
