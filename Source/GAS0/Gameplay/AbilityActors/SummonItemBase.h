// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTags.h"
#include "SummonItemBase.generated.h"

class AGAS0Character;
class UGameplayEffect;

UCLASS()
class ASummonItemBase : public AActor
{
	GENERATED_BODY()
	
public:	
	ASummonItemBase();
	
	void SetActorActive(bool bActivate) { bActorActive = bActivate; }
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void ConfirmHoldingAbility(TArray<AActor*>& FilterActors, FVector& EffectLocation) {}
	
	virtual void ApplyEffectsToFilterActors(const TArray<AActor*>& FilterActors);
	
	virtual void AuthSpawnEmitter(const FVector& SpawnLocation);

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect")
	TSoftClassPtr<UGameplayEffect> DamageEffectClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameplayCue")
	FGameplayTag DamageCueTag;
	
	UPROPERTY(ReplicatedUsing=OnRep_ActorActive)
	bool bActorActive = true;
	
	UFUNCTION()
	void OnRep_ActorActive();
	
	UPROPERTY(Transient)
	AGAS0Character* OwnerCharacter = nullptr;
	
	UPROPERTY(Transient)
	APlayerController* OwnerPlayerCharacter = nullptr;
};
