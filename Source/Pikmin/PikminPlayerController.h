// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "PikminPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class ACameraRig;

UCLASS(abstract)
class APikminPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
    APikminPlayerController();
	
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
    virtual void Tick(float DeltaSeconds) override;

private:
	///** Input Mapping Contexts */
	//UPROPERTY(EditAnywhere, Category ="Input|Input Mappings")
	//TArray<UInputMappingContext*> DefaultMappingContexts;

    /** Input mapping context */
    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputMappingContext* InputContext;

    /** Input actions */
    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputAction* MoveAction;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputAction* CameraRotateAction;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputAction* CameraZoomAction;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputAction* WhistleAction;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputAction* DismissAction;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputAction* ThrowAimAction;

    /** Cached camera rig pointer */
    UPROPERTY()
    ACameraRig* CameraRig;

private:
    void HandleMoveInput(const FInputActionValue& Value);
    void HandleCameraRotate(const FInputActionValue& Value);
    void HandleCameraZoom(const FInputActionValue& Value);
    void HandleWhistleStarted(const FInputActionValue& Value);
    void HandleWhistleCompleted(const FInputActionValue& Value);
    void HandleDismiss(const FInputActionValue& Value);
    void HandleThrowAimPressed(const FInputActionValue& Value);
    void HandleThrowAimReleased(const FInputActionValue& Value);

    /** Helper to find camera rig in world (first one or attached to pawn) */
    ACameraRig* FindCameraRig();
};
