// Copyright 2025, Rolling Pixels. All Rights Reserved

#pragma once

#include "CoreMinimal.h"

class APikminAIController;

namespace PikminBehaviors
{
	// Helpers for Pikmin behaviors
	void Idle_Update(APikminAIController* Controller, float DeltaTime);
	void Follow_Update(APikminAIController* Controller, float DeltaTime);
	void Working_Update(APikminAIController* Controller, float DeltaTime);
	void Thrown_Update(APikminAIController* Controller, float DeltaTime);
}
