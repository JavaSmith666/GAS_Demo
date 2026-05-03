// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameplayTagContainer.h"
#include "SummonItemBase.h"
#include "GameFramework/Actor.h"
#include "LaserActor.generated.h"

class AGAS0Character;
class UGameplayEffect;

UCLASS()
class ALaserActor : public ASummonItemBase
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALaserActor();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	void ClearCurrentHitCharacterDamageEffect();
	
	void SetLaserActive(bool bNewLaserActive) { bLaserActive = bNewLaserActive; }
	
public:	
	UPROPERTY(EditAnywhere)
	float LaserTraceMaxDistance = 10000.f;
	
	UPROPERTY(EditAnywhere)
	TSoftClassPtr<UGameplayEffect> LaserDamageEffect;
	
	UPROPERTY(Transient)
	AGAS0Character* CurrentHitCharacter = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ForwardImpulse = 200.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ResetFrictionDelayTime = 1.f;
	
protected:
	UPROPERTY(ReplicatedUsing=OnRep_LaserActive)
	bool bLaserActive = true;
	
	UFUNCTION()
	void OnRep_LaserActive();
	
	FActiveGameplayEffectHandle CurrentHitCharacterDamageEffectHandle;
};
