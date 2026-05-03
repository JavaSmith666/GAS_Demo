// Fill out your copyright notice in the Description page of Project Settings.

#include "Gameplay/Abilities/ChaGA_Laser.h"
#include "GAS0AbilitySystemComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Gameplay/Character/GAS0Character.h"
#include "Gameplay/AbilityActors///LaserActor.h"
#include "Gameplay/AbilityActors/SummonItemBase.h"

void UChaGA_Laser::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);
	
	ULaserConfig* LaserConfig = Cast<ULaserConfig>(RoleSkillConfig);
	if (!LaserConfig || !OwnerCharacter)
	{
		return;
	}
	
	if (OwnerCharacter->GetNetMode() < NM_Client)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = OwnerCharacter;
		SpawnParams.Instigator = OwnerCharacter;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		
		UArrowComponent* LaserPoint = OwnerCharacter->GetLaserPoint();
		if (!LaserPoint)
		{
			return;
		}
		
		SpawnedLaserActor = GetWorld()->SpawnActor<ALaserActor>(LaserConfig->SummonItemClass, FVector(), FRotator(), SpawnParams);
		if (SpawnedLaserActor)
		{
			SpawnedLaserActor->AttachToComponent(LaserPoint, FAttachmentTransformRules::SnapToTargetIncludingScale);
			SpawnedLaserActor->SetActorHiddenInGame(true);
			SpawnedLaserActor->SetActorTickEnabled(false);
			SpawnedLaserActor->SetLaserActive(false);
		}
	}
}

bool UChaGA_Laser::OnGAS0CharacterGameplayAbilityActivated(const FGameplayAbilitySpecHandle Handle,
                                                           const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                           const FGameplayEventData* TriggerEventData)
{	
	bool res = Super::OnGAS0CharacterGameplayAbilityActivated(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	ULaserConfig* LaserConfig = Cast<ULaserConfig>(RoleSkillConfig);
	if (!LaserConfig || !OwnerCharacter)
	{
		return res;
	}
	
	if (UGAS0AbilitySystemComponent* ASC = OwnerCharacter->GetAbilitySystemComponent())
	{
		ASC->AddLooseGameplayTag(LaserConfig->LaserCostTag);
		ASC->GetGameplayAttributeValueChangeDelegate(UBaseAttributeSet::GetMPAttribute()).AddUObject(this, &UChaGA_Laser::OnMPAttributeChanged);
	}
	
	if (OwnerCharacter->GetNetMode() < NM_Client)
	{
		if (SpawnedLaserActor)
		{
			SpawnedLaserActor->SetActorHiddenInGame(false);
			SpawnedLaserActor->SetActorTickEnabled(true);
			SpawnedLaserActor->SetLaserActive(true);
		}
	}
	
	if (APlayerController* PC = OwnerCharacter->GetPlayerController())
	{
		OwnerCharacter->UpdateCameraLockState(true);
	}
	
	return res;
}

void UChaGA_Laser::PreGAS0CharacterGameplayAbilityEnded(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	if (APlayerController* PC = OwnerCharacter ? OwnerCharacter->GetPlayerController() : nullptr)
	{
		OwnerCharacter->UpdateCameraLockState(false);
	}
	
	ULaserConfig* LaserConfig = Cast<ULaserConfig>(RoleSkillConfig);
	if (!LaserConfig)
	{
		return;
	}
	
	if (UGAS0AbilitySystemComponent* ASC = OwnerCharacter ? OwnerCharacter->GetAbilitySystemComponent() : nullptr)
	{
		ASC->RemoveLooseGameplayTag(LaserConfig->LaserCostTag);
	}
	
	if (OwnerCharacter->GetNetMode() < NM_Client)
	{
		if (SpawnedLaserActor)
		{
			SpawnedLaserActor->SetActorHiddenInGame(true);
			SpawnedLaserActor->SetActorTickEnabled(false);
			SpawnedLaserActor->SetLaserActive(false);
			SpawnedLaserActor->ClearCurrentHitCharacterDamageEffect();
		}
	}
	
	Super::PreGAS0CharacterGameplayAbilityEnded(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UChaGA_Laser::OnMPAttributeChanged(const FOnAttributeChangeData& Data)
{
	if (Data.NewValue <= 0.f)
	{
		if (OwnerCharacter && OwnerCharacter->GetNetMode() < NM_Client)
		{
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		}
	}
}
