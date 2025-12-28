// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PikminPlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UPikminWhistleComponent;
class UPikminThrowTargetComponent;
class UPikminInteractionComponent;
class UPikminPlayerAnimInstance;

UENUM()
enum class EPikminAimAction : uint8
{
    None,
    Pluck,
    Throw
};

UCLASS(abstract)
class APikminPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
    APikminPlayerCharacter();

    /** Camera boom */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
    USpringArmComponent* CameraBoom;

    /** Top-down style camera */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
    UCameraComponent* Camera;
    
    /** Pikmin follow target */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pikmin", meta = (AllowPrivateAccess = "true"))
    USceneComponent* FollowLocationComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pikmin")
    UPikminWhistleComponent* WhistleComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pikmin")
    UPikminThrowTargetComponent* ThrowTargetComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
    UPikminInteractionComponent* InteractionComponent;

private:
    EPikminAimAction CurrentAimAction = EPikminAimAction::None;

public:
    /** Update Pikmin state to following */
    UFUNCTION(BlueprintCallable, Category = "Gameplay")
    void CommandFollow();
    
    /** Update Pikmin state to stopped */
    UFUNCTION(BlueprintCallable, Category = "Gameplay")
    void CommandDismiss();

    UFUNCTION(BlueprintCallable, Category = "Gameplay")
    void CommandAim();

    UFUNCTION(BlueprintCallable, Category = "Gameplay")
    void CommandThrow();

    UFUNCTION(BlueprintCallable, Category = "Gameplay")
    void TryPluck();

    UFUNCTION(BlueprintCallable, Category = "Gameplay")
    void StartWhistle();

    UFUNCTION(BlueprintCallable, Category = "Gameplay")
    void EndWhistle();

    void SetMovementEnabled(bool bEnabled);

    UPikminPlayerAnimInstance* GetPikminAnimInstance() const;

private:
    void ExecuteThrow();
};

