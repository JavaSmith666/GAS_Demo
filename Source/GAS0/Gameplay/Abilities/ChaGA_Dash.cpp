// Fill out your copyright notice in the Description page of Project Settings.

#include "Gameplay/Abilities/ChaGA_Dash.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Gameplay/Character/GAS0Character.h"

bool UChaGA_Dash::OnGAS0CharacterGameplayAbilityActivated(const FGameplayAbilitySpecHandle Handle,
                                                          const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                          const FGameplayEventData* TriggerEventData)
{
	bool res = Super::OnGAS0CharacterGameplayAbilityActivated(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	if (OwnerCharacter)
	{
		OwnerCharacter->SetFrictionZero();
		if (UCharacterMovementComponent* CharacterMovement = OwnerCharacter->GetCharacterMovement())
		{
			if (UDashConfig* DashConfig = Cast<UDashConfig>(RoleSkillConfig))
			{
				FVector ForwardVector = CharacterMovement->GetForwardVector();
				CharacterMovement->AddImpulse(ForwardVector * DashConfig->ForwardImpulse, true);
			}
		}
	}
	
	return res;
}

void UChaGA_Dash::PreGAS0CharacterGameplayAbilityEnded(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	if (OwnerCharacter)
	{
		OwnerCharacter->ResetFriction();
	}
	
	Super::PreGAS0CharacterGameplayAbilityEnded(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
