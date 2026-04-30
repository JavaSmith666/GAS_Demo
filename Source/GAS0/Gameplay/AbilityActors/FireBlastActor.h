// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/AbilityActors/SummonItemBase.h"
#include "FireBlastActor.generated.h"

class AGAS0Character;

/**
 * 
 */
UCLASS()
class AFireBlastActor : public ASummonItemBase
{
	GENERATED_BODY()
	
public:
	void ScanEnemies(TArray<AGAS0Character*>& Enemies);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GroundSelect")
	float SelectRadius = 1000.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Impulse")
	float ImpulseValue = 1000.f;
};
