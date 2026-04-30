// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/Abilities/GAS0CharacterGameplayAbility.h"
#include "GAS0CharacterHoldingAbility.generated.h"

class ASummonItemBase;

/**
 * 
 */
UCLASS()
class UGAS0CharacterHoldingAbility : public UGAS0CharacterGameplayAbility
{
	GENERATED_BODY()
	
public:
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	
	void AuthConfirmHoldingAbility();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability|Animation")
	TSoftObjectPtr<UAnimMontage> HoldingMontage;
	
	virtual bool PlayFireMontage() override;
	
protected:
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
	
	virtual bool PlayHoldingMontage();
	
	void AddSkillIMC();
	
	void RemoveSkillIMC();

	UFUNCTION()
	void OnHoldingMontageCancelled();
	
	UFUNCTION()
	virtual void OnSkillConfirmed();
	
	// Authority
	UPROPERTY(Transient)
	ASummonItemBase* SummonItem = nullptr;
	
private:
	void HandleHoldingMontageEnded(bool bWasCancelled);
};
