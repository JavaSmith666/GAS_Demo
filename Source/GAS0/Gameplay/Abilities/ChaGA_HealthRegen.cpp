// Fill out your copyright notice in the Description page of Project Settings.

#include "Gameplay/Abilities/ChaGA_HealthRegen.h"
#include "GAS0AbilitySystemComponent.h"
#include "Gameplay/Character/GAS0Character.h"

void UChaGA_HealthRegen::OnGAS0CharacterGameplayAbilityActivated(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::OnGAS0CharacterGameplayAbilityActivated(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (AGAS0Character* Character = Cast<AGAS0Character>(ActorInfo->AvatarActor.Get()))
	{
		if (Character->GetNetMode() < NM_Client)
		{
			UHealthRegenConfig* HealthRegenConfig = Cast<UHealthRegenConfig>(RoleSkillConfig);
			FGameplayAbilitySpec* CurrentSpec = GetCurrentAbilitySpec();
			if (HealthRegenConfig && HealthRegenConfig->HealthRegenEffect && CurrentSpec)
			{
				if (UGAS0AbilitySystemComponent* ASC = Character->GetAbilitySystemComponent())
				{
					ASC->ApplyGameplayEffectToSelf(HealthRegenConfig->HealthRegenEffect.GetDefaultObject(), CurrentSpec->Level, MakeEffectContext(Handle, ActorInfo));
				}
			}
		}
	}
}

bool UChaGA_HealthRegen::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                            const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
                                            const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	bool res = Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
	if (!res)
	{
		return false;
	}
	
	AGAS0Character* Character = Cast<AGAS0Character>(ActorInfo->AvatarActor.Get());
	if (!Character)
	{
		return false;
	}
	
	UGAS0AbilitySystemComponent* ASC = Character->GetAbilitySystemComponent();
	if (!ASC)
	{
		return false;
	}
	
	bool found = false;
	float CurrentHealth = ASC->GetGameplayAttributeValue(UBaseAttributeSet::GetHPAttribute(), found);
	float MaxHealth = ASC->GetGameplayAttributeValue(UBaseAttributeSet::GetMaxHPAttribute(), found);
	if (CurrentHealth >= MaxHealth)
	{
		return false;
	}
	
	return true;
}
