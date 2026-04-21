// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "LaserActor.generated.h"

class AGAS0Character;
class UGameplayEffect;

UCLASS()
class GAS0_API ALaserActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALaserActor();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	void ClearCurrentHitCharacterDamageEffect();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	UPROPERTY(EditAnywhere)
	float LaserTraceMaxDistance = 10000.f;
	
	UPROPERTY(EditAnywhere)
	TSoftClassPtr<UGameplayEffect> LaserDamageEffect;
	
	UPROPERTY(EditAnywhere)
	FGameplayTag LaserDamageOnGoingTag;
	
	UPROPERTY(Transient)
	AGAS0Character* CurrentHitCharacter = nullptr;
};
