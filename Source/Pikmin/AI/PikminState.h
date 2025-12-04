// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

UENUM(BlueprintType)
enum class EPikminState : uint8
{
    Idle        UMETA(DisplayName = "Idle"),
    Following   UMETA(DisplayName = "Following"),
    Carrying    UMETA(DisplayName = "Carrying"),
    Thrown      UMETA(DisplayName = "Thrown"),
};
