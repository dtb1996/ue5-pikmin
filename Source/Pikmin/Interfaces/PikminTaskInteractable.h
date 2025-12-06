// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PikminTaskInteractable.generated.h"

class APikminCharacter;

/**
 * 
 */
UINTERFACE(Blueprintable)
class UPikminTaskInteractable : public UInterface
{
	GENERATED_BODY()
};

class IPikminTaskInteractable
{
	GENERATED_BODY()

public:
	/** Can this Pikmin take this task right now? */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Pikmin Task")
	bool CanAcceptPikmin(APikminCharacter* Pikmin);

	/** Pikmin calls this when it begins working */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Pikmin Task")
	void AssignPikmin(APikminCharacter* Pikmin);
	
	/** When a Pikmin leaves or dies */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Pikmin Task")
	void UnassignPikmin(APikminCharacter* Pikmin);
	
	/** Used to compute best distance/priority */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Pikmin Task")
	FVector GetTaskLocation() const;
};
