// Copyright Epic Games, Inc. All Rights Reserved.

#include "GAS0CharacterGameplayAbility.h"

#include "GAS0AbilitySystemComponent.h"
#include "Gameplay/Character/GAS0Character.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Animation/AnimMontage.h"

UGAS0CharacterGameplayAbility::UGAS0CharacterGameplayAbility()
{
    // Use a sensible default instancing policy for character abilities. Change as needed.
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGAS0CharacterGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	// Try to get SkillConfig from SourceObject if not already set
	if (USkillConfig* Config = Cast<USkillConfig>(Spec.SourceObject))
	{
		RoleSkillConfig = Config;
	    AbilityIndex = Config->AbilityIndex;
	    if (AGAS0Character* Character = Cast<AGAS0Character>(ActorInfo->OwnerActor.Get()))
	    {
	        OwnerCharacter = Character;
	        if (OwnerCharacter->bHasMainUICreated)
	        {
	            OnMainUICreated();
	        }
	        else
	        {
	            Character->OnMainUICreated.AddDynamic(this, &UGAS0CharacterGameplayAbility::OnMainUICreated);
	        }
	    }
	}
}

FGameplayAbilityInfo UGAS0CharacterGameplayAbility::GetAbilityInfo() const
{
    UGameplayEffect* CooldownGE = GetCooldownGameplayEffect();
    UGameplayEffect* CostGE = GetCostGameplayEffect();
    if (!CooldownGE || !CostGE)
    {
        return FGameplayAbilityInfo();
    }
    
    float CD = 0.f;
    ECostType CostType = ECostType::Default;
    float CostValue = 0.f;
    CooldownGE->DurationMagnitude.GetStaticMagnitudeIfPossible(GetCurrentAbilitySpec()->Level, CD);
    FGameplayModifierInfo ModifierInfo = CostGE->Modifiers[0];
    ModifierInfo.ModifierMagnitude.GetStaticMagnitudeIfPossible(GetCurrentAbilitySpec()->Level, CostValue);
    FString AttributeName = ModifierInfo.Attribute.AttributeName;
    if (AttributeName == "HP")
    {
        CostType = ECostType::HP;
    }
    else if (AttributeName == "MP")
    {
        CostType = ECostType::MP;
    }
    else if (AttributeName == "Strength")
    {
        CostType = ECostType::Strength;
    }
    
    return FGameplayAbilityInfo(RoleSkillConfig->AbilityIndex, CD, CostType, CostValue, AbilityMaterialInstance);
}

void UGAS0CharacterGameplayAbility::ActivateAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
    
    OnGAS0CharacterGameplayAbilityActivated(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UGAS0CharacterGameplayAbility::OnGAS0CharacterGameplayAbilityActivated(const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData)
{
    // Fallback if RoleSkillConfig is still null (e.g. for non-instanced abilities or if logic missed it)
    if (!RoleSkillConfig)
    {
        if (FGameplayAbilitySpec* Spec = GetCurrentAbilitySpec())
        {
            RoleSkillConfig = Cast<USkillConfig>(Spec->SourceObject);
        }
    }
    
    if (!ActorInfo || !RoleSkillConfig || !OwnerCharacter)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    if (!PlayFireMontage())
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }
    
    if (OwnerCharacter->GetNetMode() != NM_DedicatedServer)
    {
        StartCD();
    }
}

bool UGAS0CharacterGameplayAbility::PlayFireMontage()
{
    if (RoleSkillConfig->FireMontage == nullptr)
    {
        return false;
    }
    
    ActiveMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, RoleSkillConfig->FireMontage);
    if (!ActiveMontageTask)
    {
        return false;
    }

    ActiveMontageTask->OnCompleted.AddDynamic(this, &UGAS0CharacterGameplayAbility::OnFireMontageCompleted);
    ActiveMontageTask->OnBlendOut.AddDynamic(this, &UGAS0CharacterGameplayAbility::OnFireMontageBlendOut);
    ActiveMontageTask->OnInterrupted.AddDynamic(this, &UGAS0CharacterGameplayAbility::OnFireMontageInterrupted);
    ActiveMontageTask->OnCancelled.AddDynamic(this, &UGAS0CharacterGameplayAbility::OnFireMontageCancelled);
    ActiveMontageTask->ReadyForActivation();

    return true;
}

void UGAS0CharacterGameplayAbility::EndAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    bool bReplicateEndAbility,
    bool bWasCancelled)
{
    PreGAS0CharacterGameplayAbilityEnded(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
    
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGAS0CharacterGameplayAbility::PreGAS0CharacterGameplayAbilityEnded(const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
    bool bReplicateEndAbility, bool bWasCancelled)
{
    ActiveMontageTask = nullptr;
}

void UGAS0CharacterGameplayAbility::OnFireMontageCompleted()
{
    HandleFireMontageEnded(false);
}

void UGAS0CharacterGameplayAbility::OnFireMontageBlendOut()
{
    HandleFireMontageEnded(false);
}

void UGAS0CharacterGameplayAbility::OnFireMontageInterrupted()
{
    HandleFireMontageEnded(true);
}

void UGAS0CharacterGameplayAbility::OnFireMontageCancelled()
{
    HandleFireMontageEnded(true);
}

void UGAS0CharacterGameplayAbility::OnMainUICreated()
{
    if (OwnerCharacter)
    {
        OwnerCharacter->InitSkillIcon(GetAbilityInfo());
    }
}

void UGAS0CharacterGameplayAbility::HandleFireMontageEnded(bool bWasCancelled)
{
    if (!IsActive())
    {
        return;
    }

    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, bWasCancelled);
}

