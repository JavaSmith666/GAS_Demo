// Fill out your copyright notice in the Description page of Project Settings.

#include "Gameplay/Abilities/ChaGA_Dash.h"

#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Gameplay/Character/GAS0Character.h"

bool UChaGA_Dash::OnGAS0CharacterGameplayAbilityActivated(const FGameplayAbilitySpecHandle Handle,
                                                          const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                          const FGameplayEventData* TriggerEventData)
{
	if (!OwnerCharacter)
	{
		return false;
	}
	
	if (USphereComponent* DashDamageSphere = OwnerCharacter->GetDashDamageSphere())
	{
		DashDamageSphere->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Overlap);
	}
	
	if (UCapsuleComponent* CapsuleComponent = OwnerCharacter->GetCapsuleComponent())
	{
		CapsuleComponent->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Overlap);
	}

	if (const bool bRes = Super::OnGAS0CharacterGameplayAbilityActivated(Handle, ActorInfo, ActivationInfo, TriggerEventData); !bRes)
	{
		return false;
	}
	
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
	
	return true;
}

void UChaGA_Dash::PreGAS0CharacterGameplayAbilityEnded(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	if (OwnerCharacter)
	{
		OwnerCharacter->ResetFriction();
		OwnerCharacter->ResetDashOverlapActorsArray();
		if (USphereComponent* DashDamageSphere = OwnerCharacter->GetDashDamageSphere())
		{
			DashDamageSphere->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Ignore);
		}
	
		if (UCapsuleComponent* CapsuleComponent = OwnerCharacter->GetCapsuleComponent())
		{
			CapsuleComponent->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Block);
		}
	}
	
	Super::PreGAS0CharacterGameplayAbilityEnded(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
