// Fill out your copyright notice in the Description page of Project Settings.

#include "Gameplay/AbilityActors/SummonItemBase.h"
#include "Gameplay/Abilities/GAS0AbilitySystemComponent.h"
#include "Gameplay/Character/GAS0Character.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffect.h"
#include "Kismet/GameplayStatics.h"

ASummonItemBase::ASummonItemBase()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ASummonItemBase::BeginPlay()
{
	Super::BeginPlay();
	
	if (GetNetMode() < NM_Client)
	{
		SetReplicateMovement(true);
	}
	
	OwnerCharacter = Cast<AGAS0Character>(GetOwner());
	OwnerPlayerCharacter = OwnerCharacter ? OwnerCharacter->GetPlayerController() : nullptr;
}

void ASummonItemBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ASummonItemBase, bActorActive)
}

void ASummonItemBase::OnRep_ActorActive()
{
	SetActorTickEnabled(bActorActive);
}

void ASummonItemBase::ApplyEffectsToFilterActors(const TArray<AActor*>& FilterActors)
{
	for (AActor* Actor : FilterActors)
	{
		AGAS0Character* FilterCharacter = Cast<AGAS0Character>(Actor);
		if (!FilterCharacter)
		{
			continue;
		}
		
		if (UGAS0AbilitySystemComponent* ASC = FilterCharacter->GetAbilitySystemComponent())
		{
			UClass* DamageClass = DamageEffectClass.IsNull() ? nullptr : DamageEffectClass.LoadSynchronous();
			FGameplayEffectSpecHandle DamageEffectSpec = ASC->MakeOutgoingSpec(DamageClass, 1.f, ASC->MakeEffectContext());
			ASC->ApplyGameplayEffectSpecToSelf(*DamageEffectSpec.Data.Get());
		}
	}
}

void ASummonItemBase::AuthSpawnEmitter(const FVector& SpawnLocation)
{
	if (!OwnerCharacter)
	{
		return;
	}
	
	FGameplayCueParameters Parameters;
	Parameters.Location = SpawnLocation;
	if (UGAS0AbilitySystemComponent* ASC = OwnerCharacter->GetAbilitySystemComponent())
	{
		ASC->ExecuteGameplayCue(DamageCueTag, Parameters);
	}
}
