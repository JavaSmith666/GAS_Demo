// Fill out your copyright notice in the Description page of Project Settings.

#include "Gameplay/Abilities/ChaGA_FireBlast.h"

#include "GAS0AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Gameplay/AbilityActors/FireBlastActor.h"
#include "Gameplay/Character/GAS0Character.h"

void UChaGA_FireBlast::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);
	
	FireBlastConfig = Cast<UFireBlastConfig>(RoleSkillConfig);
	if (OwnerCharacter && RoleSkillConfig && OwnerCharacter->GetNetMode() < NM_Client)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = OwnerCharacter;
		SpawnParams.Instigator = OwnerCharacter;
		ASummonItemBase* SpawnedActor = GetWorld()->SpawnActor<ASummonItemBase>(
			RoleSkillConfig->SummonItemClass,
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			SpawnParams
		);
	
		if (SpawnedActor)
		{
			SpawnedActor->AttachToActor(OwnerCharacter, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			SpawnedActor->SetActorHiddenInGame(true);
			SpawnedActor->SetActorTickEnabled(false);
			SpawnedActor->SetActorActive(false);
			SpawnedFireBlastActor = Cast<AFireBlastActor>(SpawnedActor);
		}
	}
}

bool UChaGA_FireBlast::OnGAS0CharacterGameplayAbilityActivated(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	bool res =  Super::OnGAS0CharacterGameplayAbilityActivated(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	UAbilityTask_WaitGameplayEvent* WaitPullEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, FireBlastConfig->FireBlastPullEventTag);
	if (WaitPullEventTask)
	{
		WaitPullEventTask->EventReceived.AddDynamic(this, &UChaGA_FireBlast::OnWaitPullEventTaskReady);
		WaitPullEventTask->ReadyForActivation();
	}
	
	return res;
}

void UChaGA_FireBlast::OnWaitPullEventTaskReady(FGameplayEventData Payload)
{
	if (!SpawnedFireBlastActor || !OwnerCharacter)
	{
		return;
	}
	
	if (OwnerCharacter->GetNetMode() < NM_Client)
	{
		CacheOverlapedEnemies.Reset();
		SpawnedFireBlastActor->ScanEnemies(CacheOverlapedEnemies);
	
		// Pull Enemies
		for (auto Enemy : CacheOverlapedEnemies)
		{
			if (!Enemy)
			{
				continue;
			}
		
			FVector Direction = (OwnerCharacter->GetActorLocation() - Enemy->GetActorLocation()).GetSafeNormal();
			Enemy->PushAway(Direction, SpawnedFireBlastActor->ImpulseValue, 1.f);
		}	
	}
	
	UAbilityTask_WaitGameplayEvent* WaitPushEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, FireBlastConfig->FireBlastPushEventTag);
	if (WaitPushEventTask)
	{
		WaitPushEventTask->EventReceived.AddDynamic(this, &UChaGA_FireBlast::OnWaitPushEventTaskReady);
		WaitPushEventTask->ReadyForActivation();
	}
}

void UChaGA_FireBlast::OnWaitPushEventTaskReady(FGameplayEventData Payload)
{
	if (OwnerCharacter->GetNetMode() < NM_Client)
	{
		// Push Enemies
		for (AGAS0Character* Enemy : CacheOverlapedEnemies)
		{
			if (!Enemy)
			{
				continue;
			}
		
			FVector Direction = (Enemy->GetActorLocation() - OwnerCharacter->GetActorLocation()).GetSafeNormal();
			Enemy->PushAway(Direction, SpawnedFireBlastActor->ImpulseValue, 1.f);
			
			if (UGAS0AbilitySystemComponent* ASC = Enemy->GetAbilitySystemComponent())
			{
				FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(FireBlastConfig->FireBlastDamageEffect, 1, ASC->MakeEffectContext());
				ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			}
		}
	}
}
