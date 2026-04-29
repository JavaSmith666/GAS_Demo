// Copyright Epic Games, Inc. All Rights Reserved.

#include "GAS0CharacterGameplayAbility.h"

#include "GAS0AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Animation/AnimMontage.h"
#include "Gameplay/Character/GAS0Character.h"

UGAS0CharacterGameplayAbility::UGAS0CharacterGameplayAbility()
{
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
	    if (AGAS0Character* Character = Cast<AGAS0Character>(ActorInfo->AvatarActor.Get()))
	    {
	        OwnerCharacter = Character;
	        OwnerPlayerController = Cast<APlayerController>(OwnerCharacter->GetController());
	        if (Character->bHasMainUICreated)
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
    
    UMaterialInstance* MI = AbilityMaterialInstance.IsNull() ? nullptr : AbilityMaterialInstance.LoadSynchronous();
    return FGameplayAbilityInfo(RoleSkillConfig->AbilityIndex, CD, CostType, CostValue, MI);
}

void UGAS0CharacterGameplayAbility::ActivateAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
    
    if (!OnGAS0CharacterGameplayAbilityActivated(Handle, ActorInfo, ActivationInfo, TriggerEventData) && HasAuthority(&ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
    }
}

bool UGAS0CharacterGameplayAbility::OnGAS0CharacterGameplayAbilityActivated(const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData)
{
    if (!ActorInfo || !RoleSkillConfig || !OwnerCharacter)
    {
        return false;
    }
    
    if (OwnerCharacter->GetNetMode() < NM_Client)
    {
        if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
        {
            return false;
        }   
    }

    if (!PlayFireMontage())
    {
        return false;
    }
    
    if (OwnerCharacter->GetNetMode() != NM_DedicatedServer)
    {
        StartCD();
    }
    
    return true;
}

bool UGAS0CharacterGameplayAbility::PlayFireMontage()
{
    UAnimMontage* Montage = FireMontage.IsNull() ? nullptr : FireMontage.LoadSynchronous();
    if (!Montage)
    {
        return false;
    }
    
    ActiveMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, Montage);
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
    if (IsActive())
    {
        PreGAS0CharacterGameplayAbilityEnded(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
        Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);   
    }
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

