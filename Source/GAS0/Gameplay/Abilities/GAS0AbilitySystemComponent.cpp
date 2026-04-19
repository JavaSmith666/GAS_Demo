// Copyright Epic Games, Inc. All Rights Reserved.

#include "GAS0AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Gameplay/Character/GAS0Character.h"

UGAS0AbilitySystemComponent::UGAS0AbilitySystemComponent()
{
}

void UGAS0AbilitySystemComponent::GAS0ApplyGameplayEffectToTarget(TSubclassOf<UGameplayEffect> EffectClass, UAbilitySystemComponent* TargetASC, float Level)
{
	if (GetNetMode() == NM_Client)
	{
		Server_ApplyGameplayEffectToTarget(EffectClass, TargetASC, Level);
	}
	else
	{
		ExecuteApplyGameplayEffectToTarget(EffectClass, TargetASC, Level);
	}
}

void UGAS0AbilitySystemComponent::Server_ApplyGameplayEffectToTarget_Implementation(TSubclassOf<UGameplayEffect> EffectClass, UAbilitySystemComponent* TargetASC, float Level)
{
	ExecuteApplyGameplayEffectToTarget(EffectClass, TargetASC, Level);
}

bool UGAS0AbilitySystemComponent::Server_ApplyGameplayEffectToTarget_Validate(TSubclassOf<UGameplayEffect> EffectClass, UAbilitySystemComponent* TargetASC, float Level)
{
	return true;
}

void UGAS0AbilitySystemComponent::ExecuteApplyGameplayEffectToTarget(TSubclassOf<UGameplayEffect> EffectClass, UAbilitySystemComponent* TargetASC, float Level)
{
	if (EffectClass && TargetASC)
	{
		FGameplayEffectContextHandle EffectContext = MakeEffectContext();
		EffectContext.AddInstigator(GetOwner(), GetOwner());
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingSpec(EffectClass, Level, EffectContext);
		if (SpecHandle.IsValid())
		{
			ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
		}
	}
}