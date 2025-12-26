// Copyright 2025, Rolling Pixels. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Types/ItemTypes.h"
#include "PikminTaskInteractable.generated.h"

class APikminCharacter;

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
	
	/** When a task is completed */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Pikmin Task")
	void HandleTaskCompleted();

	/** Get the item type for this object */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Pikmin Task")
	EItemType GetItemType() const;

	/** Get Pikmin yield for this object */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Pikmin Task")
	int32 GetPikminYield() const;
};
