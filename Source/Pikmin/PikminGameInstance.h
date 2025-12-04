// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "PikminGameInstance.generated.h"

class APikminCharacter;

/**
 * 
 */
UCLASS(Abstract)
class PIKMIN_API UPikminGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<APikminCharacter> DefaultPikminClass;
	
};
