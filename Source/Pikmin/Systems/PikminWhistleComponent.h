// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PikminWhistleComponent.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent) )
class PIKMIN_API UPikminWhistleComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPikminWhistleComponent();

protected:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTick) override;

public:
	UFUNCTION(BlueprintCallable)
	void StartWhistle();

	UFUNCTION(BlueprintCallable)
	void EndWhistle();

private:
	UPROPERTY(EditAnywhere)
	float StartRadius = 50.0f;

	UPROPERTY(EditAnywhere)
	float MaxRadius = 800.0f;

	UPROPERTY(EditAnywhere)
	float ExpansionSpeed = 600.0f;

	float CurrentRadius = 0.0f;

	bool bIsWhistling = false;

	//TArray<AActor*> SelectedPikmin;

protected:
	/** Debug */
	UPROPERTY(EditAnywhere, Category = "Camera|Debug")
	bool bDrawDebug = false;
};
