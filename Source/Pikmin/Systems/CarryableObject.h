// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/PikminTaskInteractable.h"
#include "ItemTypes.h"
#include "CarryableObject.generated.h"

class APikminCharacter;

UCLASS()
class PIKMIN_API ACarryableObject : public AActor, public IPikminTaskInteractable
{
	GENERATED_BODY()
	
public:	
	ACarryableObject();

protected:
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	virtual void Tick(float DeltaTime) override;

	// ---- Interface Functions ----
	virtual bool CanAcceptPikmin_Implementation(APikminCharacter* Pikmin) override;
	virtual void AssignPikmin_Implementation(APikminCharacter* Pikmin) override;
	virtual void UnassignPikmin_Implementation(APikminCharacter* Pikmin) override;
	virtual FVector GetTaskLocation_Implementation() const override;

	UFUNCTION(BlueprintCallable)
	void HandleDelivered();

protected:
	/** Returns a snap position in the ring */
	FVector GetPikminAttachPoint(int32 Index) const;

	/** Updates transform of assigned pikmin */
	void UpdatePikminPositions();

	/** Moves this object once enough pikmin are attached */
	void UpdateMovement(float DeltaTime);

private:
	// ---- Task Settings ----
	UPROPERTY(EditAnywhere, Category = "Task")
	EItemType ItemType;

	UPROPERTY(EditAnywhere, Category = "Task")
	int RequiredPikmin = 3;

	UPROPERTY(EditAnywhere, Category = "Task")
	int MaxPikmin = 8;

	UPROPERTY(EditAnywhere, Category = "Task")
	float AttachRadius = 70.0f;

	UPROPERTY(EditAnywhere, Category = "Task")
	float BaseMoveSpeed = 120.0f;

	UPROPERTY(EditAnywhere, Category = "Task")
	bool bAutoRegisterToSubsystem = true;

	// Target
	UPROPERTY()
	AActor* DeliveryTarget;

	// ---- State ----
	UPROPERTY(VisibleAnywhere)
	TArray<TWeakObjectPtr<APikminCharacter>> AssignedPikmin;

	bool bIsMoving = false;

public:
	void SetDeliveryTarget(AActor* Target) { DeliveryTarget = Target; };
};
