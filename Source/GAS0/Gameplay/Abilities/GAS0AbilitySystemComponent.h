// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GAS0AbilitySystemComponent.generated.h"

/**
 * Custom Ability System Component for GAS_Demo.
 */
UCLASS()
class GAS0_API UGAS0AbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	UGAS0AbilitySystemComponent();

	/** 
	 * 蓝图调用的入口函数（路由函数）
	 * 内部处理：客户端发RPC，服务端/Standalone直接执行
	 */
	UFUNCTION(BlueprintCallable, Category = "GAS0|Abilities")
	void ApplyGameplayEffectToTarget(TSubclassOf<UGameplayEffect> EffectClass, UAbilitySystemComponent* TargetASC, float Level = 1.0f);

protected:

	/** 
	 * 服务端执行的 RPC
	 * 只有在客户端调用时才会通过网络发往服务器
	 */
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_ApplyGameplayEffectToTarget(TSubclassOf<UGameplayEffect> EffectClass, UAbilitySystemComponent* TargetASC, float Level);

	/** 真正的执行逻辑，被路由函数和 RPC 共同调用 */
	void ExecuteApplyGameplayEffectToTarget(TSubclassOf<UGameplayEffect> EffectClass, UAbilitySystemComponent* TargetASC, float Level);
};
