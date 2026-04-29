// Copyright Epic Games, Inc. All Rights Reserved.

#include "GAS0AbilitySystemComponent.h"
#include "GAS0CharacterHoldingAbility.h"

UGAS0AbilitySystemComponent::UGAS0AbilitySystemComponent()
{
}

void UGAS0AbilitySystemComponent::ServerConfirmHoldingAbility_Implementation(const FGameplayAbilitySpecHandle& Handle)
{
	FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(Handle);
	if (!AbilitySpec)
	{
		return;
	}
	
	if (!AbilitySpec->IsActive())
	{
		return;
	}
	
	UGAS0CharacterHoldingAbility* HoldingAbility = nullptr;
	for (UGameplayAbility* ActiveAbility : AbilitySpec->GetAbilityInstances())
	{
		if (ActiveAbility && ActiveAbility->GetCurrentActorInfo() != nullptr)
		{
			HoldingAbility = Cast<UGAS0CharacterHoldingAbility>(ActiveAbility);
			if (HoldingAbility)
			{
				HoldingAbility->AuthConfirmHoldingAbility();
				break;
			}
		}
	}
}

bool UGAS0AbilitySystemComponent::ServerConfirmHoldingAbility_Validate(const FGameplayAbilitySpecHandle& Handle)
{
	return true;
}
