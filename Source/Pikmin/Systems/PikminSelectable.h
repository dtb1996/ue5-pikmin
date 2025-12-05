// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PikminSelectable.generated.h"

/**
 * 
 */
UINTERFACE(Blueprintable)
class UPikminSelectable : public UInterface
{
	GENERATED_BODY()
};

class IPikminSelectable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void OnWhistleSelect(AActor* Caller);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void OnWhistleDeselect(AActor* Caller);
};
