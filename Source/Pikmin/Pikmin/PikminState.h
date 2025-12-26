// Copyright 2025, Rolling Pixels. All Rights Reserved

#pragma once

UENUM(BlueprintType)
enum class EPikminState : uint8
{
    Idle            UMETA(DisplayName = "Idle"),
    Following       UMETA(DisplayName = "Following"),
    Carrying        UMETA(DisplayName = "Carrying"),
    Thrown          UMETA(DisplayName = "Thrown"),
    Working         UMETA(DisplayName = "Working"),
    MovingToTask    UMETA(DisplayName = "MovingToTask"),
};
