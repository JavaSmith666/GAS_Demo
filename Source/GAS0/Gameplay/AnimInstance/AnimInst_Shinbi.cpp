// Fill out your copyright notice in the Description page of Project Settings.

#include "Gameplay/AnimInstance/AnimInst_Shinbi.h"

#include "Engine/OverlapResult.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Gameplay/Abilities/GAS0AbilitySystemComponent.h"
#include "Gameplay/Character/GAS0Character.h"
#include "Math/UnrealMathUtility.h"

void UAnimInst_Shinbi::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	
	OwnerCharacter = Cast<AGAS0Character>(TryGetPawnOwner());
}

void UAnimInst_Shinbi::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);
	
	if (OwnerCharacter)
	{
		FVector Velocity = OwnerCharacter->GetVelocity();
		Speed = Velocity.Size2D();
		Angle = CalculateDirection(Velocity, OwnerCharacter->GetActorRotation());
		if (UCharacterMovementComponent* MoveComp = OwnerCharacter->GetCharacterMovement())
		{
			bInAir = MoveComp->IsFalling();
		}
	}
}

void UAnimInst_Shinbi::OnStartMontageNotify()
{
	bNeedMontage = true;
	USkeletalMeshComponent* SkeletalMeshComp = OwnerCharacter->GetMesh();
	if (!SkeletalMeshComp || !OwnerCharacter || OwnerCharacter->GetNetMode() == NM_Client)
	{
		return;
	}
	
	FVector StartLocation = SkeletalMeshComp->GetBoneLocation(StartAttackBoneName);
	FVector OwnerCharacterForwardVector = OwnerCharacter->GetActorForwardVector();
	FCollisionShape CollisionShape = FCollisionShape::MakeCapsule(AttackCapsuleRadius, AttackCapsuleHalfHeight);
	TArray<FOverlapResult> OutOverlaps;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(OwnerCharacter);
	
	TSet<AGAS0Character*> OverlappingCharacters;
	GetWorld()->OverlapMultiByObjectType(OutOverlaps, StartLocation, FQuat::Identity, ECC_Pawn, CollisionShape, QueryParams);
	for (const FOverlapResult& OverlapResult : OutOverlaps)
	{
		AActor* HitActor = OverlapResult.GetActor();
		AGAS0Character* HitCharacter = Cast<AGAS0Character>(HitActor);
		if (!HitCharacter)
		{
			continue;
		}
		
		if (OverlappingCharacters.Contains(HitCharacter))
		{
			continue;
		}
		
		if (HitCharacter->GetTeamID() == OwnerCharacter->GetTeamID())
		{
			continue;
		}
		
		OverlappingCharacters.Add(HitCharacter);
		FVector SourceToTargetVector = HitActor->GetActorLocation() - OwnerCharacter->GetActorLocation();
		if (OwnerCharacterForwardVector.Dot(SourceToTargetVector) <= 0.f)
		{
			continue;
		}
		
		if (UGAS0AbilitySystemComponent* ASC = HitCharacter->GetAbilitySystemComponent())
		{
			UClass* GEClass = MeleeDamageEffect.IsNull() ? nullptr : MeleeDamageEffect.LoadSynchronous();
			UGameplayEffect* DamageEffect = GEClass ? GEClass->GetDefaultObject<UGameplayEffect>() : nullptr;
			ASC->ApplyGameplayEffectToSelf(DamageEffect, 1.f, ASC->MakeEffectContext());
		}
	}
}

void UAnimInst_Shinbi::OnEndMontageNotify()
{
	bNeedMontage = false;
}
