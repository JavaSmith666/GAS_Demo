// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Character/GAS0AICharacter.h"

#include "Gameplay/Abilities/GAS0AbilitySystemComponent.h"

AGAS0AICharacter::AGAS0AICharacter()
{
	
}

void AGAS0AICharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	}
}
