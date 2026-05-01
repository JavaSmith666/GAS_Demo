// Fill out your copyright notice in the Description page of Project Settings.

#include "Gameplay/AbilityActors/FireBlastActor.h"
#include "Engine/OverlapResult.h"
#include "Gameplay/Character/GAS0Character.h"

void AFireBlastActor::ScanEnemies(TArray<AGAS0Character*>& Enemies)
{
	Enemies.Reset();
	
	FVector StartLocation = OwnerCharacter->GetActorLocation();
	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);
	CollisionParams.AddIgnoredActor(OwnerCharacter);
	FCollisionShape CollisionShape = FCollisionShape::MakeSphere(SelectRadius);
	
	bool bHit = GetWorld()->OverlapMultiByObjectType(
		OverlapResults,
		StartLocation,
		FQuat::Identity,
		ECC_Pawn,
		CollisionShape,
		CollisionParams
	);
	
	for (const FOverlapResult& OverlapResult : OverlapResults)
	{
		AGAS0Character* HitCharacter = Cast<AGAS0Character>(OverlapResult.GetActor());
		if (!HitCharacter || HitCharacter == OwnerCharacter || HitCharacter->GetTeamID() == OwnerCharacter->GetTeamID() || HitCharacter->IsDead())
		{
			continue;
		}
		
		Enemies.AddUnique(HitCharacter);
	}
}

