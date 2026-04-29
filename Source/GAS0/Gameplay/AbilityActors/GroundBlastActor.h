// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SummonItemBase.h"
#include "GroundBlastActor.generated.h"

class AGAS0Character;
class APlayerController;
class UDecalComponent;

/**
 * 
 */
UCLASS()
class AGroundBlastActor : public ASummonItemBase
{
	GENERATED_BODY()
	
public:
	AGroundBlastActor();
	
	virtual void Tick(float DeltaTime) override;
	
	virtual void ConfirmHoldingAbility(TArray<AActor*>& FilterActors, FVector& EffectLocation) override;

	bool GetPlayerLookAtPoint(FVector& Out_LookAtPoint);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GroundSelect")
	float SelectRadius = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GroundSelect")
	float LineTracemaxDistance = 10000.f;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Component")
	UDecalComponent* DecalComponent = nullptr;
};
