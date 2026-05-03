// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/AbilityActors//LaserActor.h"
#include "Gameplay/Abilities/GAS0AbilitySystemComponent.h"
#include "AbilitySystemComponent.h"
#include "Gameplay/Character/GAS0Character.h"
#include "GameplayEffect.h"
#include "Components/ArrowComponent.h"
#include "Net/UnrealNetwork.h"

ALaserActor::ALaserActor()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

void ALaserActor::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ALaserActor, bLaserActive);
}

void ALaserActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (GetNetMode() < NM_Client)
	{
		if (!OwnerCharacter || !OwnerCharacter->LaserPoint)
		{
			return;
		}
		
		const FVector StartLocation = OwnerCharacter->LaserPoint->GetComponentLocation();
		const FVector ForwardDir = OwnerCharacter->GetActorForwardVector();
		const FVector EndLocation = StartLocation + ForwardDir * LaserTraceMaxDistance;
		
		FHitResult HitResult;		
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);
		QueryParams.AddIgnoredActor(OwnerCharacter);

		if (bool bHit = GetWorld()->LineTraceSingleByObjectType(HitResult, StartLocation, EndLocation, ECC_Pawn, QueryParams))
		{
			AActor* HitActor = HitResult.GetActor();
			if (AGAS0Character* HitCharacter = Cast<AGAS0Character>(HitActor))
			{
				if (!HitCharacter->IsDead() && HitCharacter != CurrentHitCharacter && HitCharacter->GetTeamID() != OwnerCharacter->GetTeamID())
				{
					ClearCurrentHitCharacterDamageEffect();
					CurrentHitCharacter = HitCharacter;
					if (UAbilitySystemComponent* HitCharacterASC = HitCharacter->GetAbilitySystemComponent())
					{
						UClass* GEClass = LaserDamageEffect.IsNull() ? nullptr : LaserDamageEffect.LoadSynchronous();
						const UGameplayEffect* TempGE = GEClass ? GEClass->GetDefaultObject<UGameplayEffect>() : nullptr;
						FGameplayEffectSpecHandle SpecHandle = HitCharacterASC->MakeOutgoingSpec(TempGE->GetClass(), 1.f, HitCharacterASC->MakeEffectContext());
						CurrentHitCharacterDamageEffectHandle = HitCharacterASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
					}
				}
				
				FVector Dir = (HitCharacter->GetActorLocation() - OwnerCharacter->GetActorLocation()).GetSafeNormal();
				HitCharacter->PushAway(Dir, ForwardImpulse, ResetFrictionDelayTime);
				return;
			}
		}
		
		ClearCurrentHitCharacterDamageEffect();
	}
}

void ALaserActor::ClearCurrentHitCharacterDamageEffect()
{
	if (CurrentHitCharacter)
	{
		if (UAbilitySystemComponent* CurrentASC = CurrentHitCharacter->GetAbilitySystemComponent())
		{
			CurrentASC->RemoveActiveGameplayEffect(CurrentHitCharacterDamageEffectHandle);
		}
	}
	
	CurrentHitCharacter = nullptr;
}

void ALaserActor::OnRep_LaserActive()
{
	SetActorTickEnabled(bLaserActive);
}
