// Copyright Epic Games, Inc. All Rights Reserved.

#include "GAS0CharacterGameplayAbility.h"
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
	}
}

void UGAS0CharacterGameplayAbility::ActivateAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    // Fallback if RoleSkillConfig is still null (e.g. for non-instanced abilities or if logic missed it)
    if (!RoleSkillConfig)
    {
	    if (FGameplayAbilitySpec* Spec = GetCurrentAbilitySpec())
	    {
		    RoleSkillConfig = Cast<USkillConfig>(Spec->SourceObject);
	    }
    }
    
    if (!ActorInfo || !RoleSkillConfig)
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
    ActiveMontageTask = nullptr;
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
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

void UGAS0CharacterGameplayAbility::HandleFireMontageEnded(bool bWasCancelled)
{
    if (!IsActive())
    {
        return;
    }

    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, bWasCancelled);
}

