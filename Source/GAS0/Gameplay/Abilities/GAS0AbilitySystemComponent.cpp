// Copyright Epic Games, Inc. All Rights Reserved.

#include "GAS0AbilitySystemComponent.h"

UGAS0AbilitySystemComponent::UGAS0AbilitySystemComponent()
{
}

void UGAS0AbilitySystemComponent::ApplyGameplayEffectToTarget(TSubclassOf<UGameplayEffect> EffectClass, UAbilitySystemComponent* TargetASC, float Level)
{
	// 如果已经在服务端（包括 Standalone）
	if (GetNetMode() == NM_Client)
	{
		// 正在客户端，发 RPC 到服务器执行
		Server_ApplyGameplayEffectToTarget(EffectClass, TargetASC, Level);
	}
	else
	{
		ExecuteApplyGameplayEffectToTarget(EffectClass, TargetASC, Level);
	}
}

void UGAS0AbilitySystemComponent::Server_ApplyGameplayEffectToTarget_Implementation(TSubclassOf<UGameplayEffect> EffectClass, UAbilitySystemComponent* TargetASC, float Level)
{
	// RPC 在服务端收到，执行逻辑
	ExecuteApplyGameplayEffectToTarget(EffectClass, TargetASC, Level);
}

bool UGAS0AbilitySystemComponent::Server_ApplyGameplayEffectToTarget_Validate(TSubclassOf<UGameplayEffect> EffectClass, UAbilitySystemComponent* TargetASC, float Level)
{
	// RPC 验证，通常返回 true，如果有资产检查可以放这里
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
			// 只有在服务端才真正应用 Effect
			ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
		}
	}
}
