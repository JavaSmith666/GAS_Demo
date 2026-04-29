// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Abilities/ChaGA_GroundBlast.h"

bool UChaGA_GroundBlast::OnGAS0CharacterGameplayAbilityActivated(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	bool res = Super::OnGAS0CharacterGameplayAbilityActivated(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	AddSkillIMC();
	return res;
}

void UChaGA_GroundBlast::PreGAS0CharacterGameplayAbilityEnded(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	RemoveSkillIMC();
	Super::PreGAS0CharacterGameplayAbilityEnded(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UChaGA_GroundBlast::OnSkillConfirmed()
{
	Super::OnSkillConfirmed();
}
