// Fill out your copyright notice in the Description page of Project Settings.

#include "Gameplay/AbilityActors/GroundBlastActor.h"
#include "Components/DecalComponent.h"
#include "Engine/OverlapResult.h"
#include "Gameplay/Character/GAS0Character.h"

AGroundBlastActor::AGroundBlastActor()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	
	DecalComponent = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComponent"));
	DecalComponent->SetupAttachment(RootComponent);
}

void AGroundBlastActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (OwnerCharacter && OwnerCharacter->GetNetMode() < NM_Client)
	{
		FVector LookAtLocation = FVector::ZeroVector;
		GetPlayerLookAtPoint(LookAtLocation);
		SetActorLocation(LookAtLocation);
	}
}

void AGroundBlastActor::ConfirmHoldingAbility(TArray<AActor*>& FilterActors, FVector& EffectLocation)
{
	Super::ConfirmHoldingAbility(FilterActors, EffectLocation);
	
	FilterActors.Reset();
	GetPlayerLookAtPoint(EffectLocation);
	CacheViewPoint = EffectLocation;
	
	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);
	CollisionParams.AddIgnoredActor(OwnerCharacter);
	FCollisionShape CollisionShape = FCollisionShape::MakeSphere(SelectRadius);
	
	bool bHit = GetWorld()->OverlapMultiByObjectType(
		OverlapResults,
		EffectLocation,
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
		
		FilterActors.AddUnique(HitCharacter);
	}
}

void AGroundBlastActor::ApplyEffectsToFilterActors(const TArray<AActor*>& FilterActors)
{
	Super::ApplyEffectsToFilterActors(FilterActors);
	
	if (!OwnerCharacter)
	{
		return;
	}
	
	for (AActor* Actor : FilterActors)
	{
		AGAS0Character* FilterCharacter = Cast<AGAS0Character>(Actor);
		if (!FilterCharacter)
		{
			continue;
		}
		
		FVector HorizontalDir = FilterCharacter->GetActorLocation() - CacheViewPoint;
		HorizontalDir.Z = 0.f;
		FVector ImpulseDir = (HorizontalDir + ImpulseUpVector).GetSafeNormal();
		FilterCharacter->PushAway(ImpulseDir, ImpulseValue, 1.f);
	}
}

bool AGroundBlastActor::GetPlayerLookAtPoint(FVector& Out_LookAtPoint)
{
	FVector ViewStartLocation = FVector::ZeroVector;
	FRotator ViewRotation = FRotator::ZeroRotator;
	if (OwnerPlayerCharacter)
	{
		OwnerPlayerCharacter->GetPlayerViewPoint(ViewStartLocation, ViewRotation);
	}
	
	FVector EndLocation = ViewStartLocation + ViewRotation.Vector() * LineTracemaxDistance;
	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(OwnerCharacter);
	
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, ViewStartLocation, EndLocation, ECC_Visibility, CollisionParams);
	if (bHit)
	{
		Out_LookAtPoint = HitResult.ImpactPoint;
	}
	
	return bHit;
}
