// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/Abilities/GAS0CharacterGameplayAbility.h"
#include "ChaGA_Laser.generated.h"

class ALaserActor;

/**
 * 
 */
UCLASS()
class GAS0_API UChaGA_Laser : public UGAS0CharacterGameplayAbility
{
	GENERATED_BODY()
	
public:
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

protected:
	virtual void OnGAS0CharacterGameplayAbilityActivated(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;
	
	virtual void PreGAS0CharacterGameplayAbilityEnded(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled) override;
	
	UPROPERTY(Transient)
	ALaserActor* SpawnedLaserActor = nullptr;
};
