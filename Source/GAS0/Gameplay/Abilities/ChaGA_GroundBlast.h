// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/Abilities/GAS0CharacterHoldingAbility.h"
#include "ChaGA_GroundBlast.generated.h"

/**
 * 
 */
UCLASS()
class UChaGA_GroundBlast : public UGAS0CharacterHoldingAbility
{
	GENERATED_BODY()
	
public:
	virtual bool OnGAS0CharacterGameplayAbilityActivated(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;
	
	virtual void PreGAS0CharacterGameplayAbilityEnded(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled);
	
protected:
	virtual void OnSkillConfirmed() override;
};
