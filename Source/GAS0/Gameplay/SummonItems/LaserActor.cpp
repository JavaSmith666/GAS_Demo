// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/SummonItems/LaserActor.h"
#include "Gameplay/Abilities/GAS0AbilitySystemComponent.h"
#include "AbilitySystemComponent.h"
#include "Gameplay/Character/GAS0Character.h"
#include "GameplayEffect.h"
#include "Components/ArrowComponent.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
ALaserActor::ALaserActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ALaserActor::BeginPlay()
{
	Super::BeginPlay();
	SetActorTickEnabled(false);
}

// Called every frame
void ALaserActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (GetNetMode() < NM_Client)
	{
		AGAS0Character* OwnerCharacter = Cast<AGAS0Character>(GetOwner());
		if (!OwnerCharacter || !OwnerCharacter->LaserPoint)
		{
			return;
		}
		
		const FVector StartLocation = OwnerCharacter->LaserPoint->GetComponentLocation();
		const FVector Dir = OwnerCharacter->GetActorForwardVector();
		const FVector EndLocation = StartLocation + Dir * LaserTraceMaxDistance;
		
		FHitResult HitResult;
		TArray<AActor*> ActorsToIgnore;
		ActorsToIgnore.Add(this);
		ActorsToIgnore.Add(OwnerCharacter);
		
		bool bHit = UKismetSystemLibrary::LineTraceSingle(this, StartLocation, EndLocation, UEngineTypes::ConvertToTraceType(ECC_Pawn), false, ActorsToIgnore, EDrawDebugTrace::None, HitResult, true);
		if (bHit && HitResult.GetActor())
		{
			if (AGAS0Character* HitCharacter = Cast<AGAS0Character>(HitResult.GetActor()))
			{
				if (!HitCharacter->IsDead() && HitCharacter != CurrentHitCharacter)
				{
					ClearCurrentHitCharacterDamageEffect();
					CurrentHitCharacter = HitCharacter;
					if (UAbilitySystemComponent* HitCharacterASC = HitCharacter->GetAbilitySystemComponent())
					{
						HitCharacterASC->AddLooseGameplayTag(LaserDamageOnGoingTag);
						UClass* GEClass = LaserDamageEffect.LoadSynchronous();
						const UGameplayEffect* TempGE = GEClass ? GEClass->GetDefaultObject<UGameplayEffect>() : nullptr;
						HitCharacterASC->ApplyGameplayEffectToSelf(TempGE, 1.f, HitCharacterASC->MakeEffectContext());
					}
				}
				
				HitCharacter->PushAway(OwnerCharacter, ForwardImpulse, ResetFrictionDelayTime);
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
			CurrentASC->RemoveLooseGameplayTag(LaserDamageOnGoingTag);
		}
	}
	
	CurrentHitCharacter = nullptr;
}
