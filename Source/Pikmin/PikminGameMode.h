// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PikminGameMode.generated.h"

/**
 *  Simple GameMode for a third person game
 */
UCLASS(abstract)
class APikminGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	
	/** Constructor */
	APikminGameMode();
};



