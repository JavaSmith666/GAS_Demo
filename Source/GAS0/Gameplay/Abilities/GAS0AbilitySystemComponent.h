// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GAS0AbilitySystemComponent.generated.h"

/**
 * Custom Ability System Component for GAS_Demo.
 */
UCLASS()
class UGAS0AbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	UGAS0AbilitySystemComponent();
	
	UFUNCTION(BlueprintCallable, Category = "GAS0|Abilities")
	void GAS0ApplyGameplayEffectToTarget(TSubclassOf<UGameplayEffect> EffectClass, UAbilitySystemComponent* TargetASC, float Level = 1.0f);

protected:
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_ApplyGameplayEffectToTarget(TSubclassOf<UGameplayEffect> EffectClass, UAbilitySystemComponent* TargetASC, float Level);
	
	void ExecuteApplyGameplayEffectToTarget(TSubclassOf<UGameplayEffect> EffectClass, UAbilitySystemComponent* TargetASC, float Level);
};
