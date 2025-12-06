// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PikminPlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class ACameraRig;
class UPikminWhistleComponent;

UCLASS(abstract)
class APikminPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
    APikminPlayerCharacter();

protected:
	virtual void BeginPlay() override;

public:
    /** Camera boom */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
    USpringArmComponent* CameraBoom;

    /** Top-down style camera */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
    USceneComponent* FollowLocationComponent;
    
    /** Pikmin follow target */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
    UCameraComponent* Camera;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UPikminWhistleComponent* WhistleComponent;

public:
    /** Spawn or attach a camera rig to this pawn (optional helper) */
    UFUNCTION(BlueprintCallable, Category = "Camera")
    ACameraRig* SpawnAndAttachCameraRig();

    /** Update Pikmin state to following */
    UFUNCTION(BlueprintCallable, Category = "Gameplay")
    void CommandFollow();
    
    /** Update Pikmin state to stopped */
    UFUNCTION(BlueprintCallable, Category = "Gameplay")
    void CommandDismiss();

    UFUNCTION(BlueprintCallable, Category = "Gameplay")
    void CommandThrow();

protected:   
    /** Camera rig class to spawn */
    UPROPERTY(EditDefaultsOnly, Category = "Camera")
    TSubclassOf<ACameraRig> CameraRigClass;

    /** Runtime pointer to spawned rig */
    UPROPERTY()
    ACameraRig* SpawnedCameraRig;

private:
    FVector GetThrowAimPoint() const;
};

