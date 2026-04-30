#include "GAS0CharacterHoldingAbility.h"
#include "EnhancedInputSubsystems.h"
#include "GAS0AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "Gameplay/Character/GAS0Character.h"
#include "Gameplay/Character/GAS0CharacterGlobalConfig.h"
#include "Gameplay/AbilityActors//GroundBlastActor.h"

void UGAS0CharacterHoldingAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo,
                                                 const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);
	
	if (RoleSkillConfig && OwnerCharacter->GetNetMode() < NM_Client)
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
			SpawnedActor->SetActorHiddenInGame(true);
			SpawnedActor->SetActorTickEnabled(false);
			SpawnedActor->SetActorActive(false);
			SummonItem = SpawnedActor;
		}
	}
}

void UGAS0CharacterHoldingAbility::AuthConfirmHoldingAbility()
{
	UGAS0AbilitySystemComponent* ASC = OwnerCharacter ? OwnerCharacter->GetAbilitySystemComponent() : nullptr;
	if (!ASC)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}
	
	FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
	ASC->ApplyGameplayEffectToSelf(GetCostGameplayEffect(), 1, Context);
	ASC->ApplyGameplayEffectToSelf(GetCooldownGameplayEffect(), 1, Context);
	if (SummonItem)
	{
		TArray<AActor*> FilterActors;
		FVector SpawnEmitterLocation;
		SummonItem->ConfirmHoldingAbility(FilterActors, SpawnEmitterLocation);
		SummonItem->ApplyEffectsToFilterActors(FilterActors);
		SummonItem->AuthSpawnEmitter(SpawnEmitterLocation);
	}
	
	if (!PlayFireMontage())
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}
}

bool UGAS0CharacterHoldingAbility::PlayFireMontage()
{
	if (SummonItem && SummonItem->GetNetMode() < NM_Client)
	{
		SummonItem->SetActorHiddenInGame(true);
		SummonItem->SetActorTickEnabled(false);
		SummonItem->SetActorActive(false);
	}
	
	return Super::PlayFireMontage();
}

bool UGAS0CharacterHoldingAbility::OnGAS0CharacterGameplayAbilityActivated(
	const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!ActorInfo || !RoleSkillConfig || !OwnerCharacter)
	{
		return false;
	}
	
	if (!PlayHoldingMontage())
	{
		return false;
	}
	
	OwnerCharacter->UpdateCameraLockState(true);
	
	if (OwnerCharacter->GetNetMode() != NM_DedicatedServer)
	{
		OwnerCharacter->OnSkillConfirmed.BindUObject(this, &UGAS0CharacterHoldingAbility::OnSkillConfirmed);	
	}
	
	if (OwnerCharacter->GetNetMode() < NM_Client && SummonItem)
	{
		SummonItem->SetActorHiddenInGame(false);
		SummonItem->SetActorTickEnabled(true);
		SummonItem->SetActorActive(true);
	}
	
	return true;
}

void UGAS0CharacterHoldingAbility::PreGAS0CharacterGameplayAbilityEnded(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	if (APlayerController* PC = OwnerCharacter->GetPlayerController())
	{
		OwnerCharacter->UpdateCameraLockState(false);
	}
	
	if (OwnerCharacter->GetNetMode() != NM_DedicatedServer)
	{
		OwnerCharacter->OnSkillConfirmed.Unbind();
	}
	
	Super::PreGAS0CharacterGameplayAbilityEnded(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

bool UGAS0CharacterHoldingAbility::PlayHoldingMontage()
{
	UAnimMontage* Montage = HoldingMontage.IsNull() ? nullptr : HoldingMontage.LoadSynchronous();
	if (!Montage)
	{
		return false;
	}
	
	ActiveMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, Montage);
	if (!ActiveMontageTask)
	{
		return false;
	}
	
	ActiveMontageTask->OnCancelled.AddDynamic(this, &UGAS0CharacterHoldingAbility::OnHoldingMontageCancelled);
	ActiveMontageTask->ReadyForActivation();

	return true;
}

void UGAS0CharacterHoldingAbility::AddSkillIMC()
{
	if (OwnerPlayerController && OwnerPlayerController->IsLocalPlayerController())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(OwnerPlayerController->GetLocalPlayer()))
		{
			UGAS0CharacterGlobalConfig* CharacterGlobalConfig = OwnerCharacter->GetGAS0CharacterGlobalConfig();
			for (UInputMappingContext* CurrentContext : CharacterGlobalConfig->SkillMappingContexts)
			{
				Subsystem->AddMappingContext(CurrentContext, 1);
			}
		}
	}
}

void UGAS0CharacterHoldingAbility::RemoveSkillIMC()
{
	if (OwnerPlayerController && OwnerPlayerController->IsLocalPlayerController())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(OwnerPlayerController->GetLocalPlayer()))
		{
			UGAS0CharacterGlobalConfig* CharacterGlobalConfig = OwnerCharacter->GetGAS0CharacterGlobalConfig();
			for (UInputMappingContext* CurrentContext : CharacterGlobalConfig->SkillMappingContexts)
			{
				Subsystem->RemoveMappingContext(CurrentContext);
			}
		}
	}
}

void UGAS0CharacterHoldingAbility::OnHoldingMontageCancelled()
{
	HandleHoldingMontageEnded(true);
}

void UGAS0CharacterHoldingAbility::OnSkillConfirmed()
{
	if (!PlayFireMontage())
	{
		return;
	}
	
	StartCD();
	
	if (OwnerCharacter->GetNetMode() == NM_Client)
	{
		if (UGAS0AbilitySystemComponent* ASC = OwnerCharacter->GetAbilitySystemComponent())
		{
			ASC->ServerConfirmHoldingAbility(CurrentSpecHandle);
		}
	}
	else
	{
		AuthConfirmHoldingAbility();
	}
}

void UGAS0CharacterHoldingAbility::HandleHoldingMontageEnded(bool bWasCancelled)
{
	if (!IsActive())
	{
		return;
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, bWasCancelled);
}