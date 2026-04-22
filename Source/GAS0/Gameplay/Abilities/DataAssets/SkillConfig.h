#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "SkillConfig.generated.h"

class UGameplayEffect;
class ALaserActor;

/**
 * Base data asset for all skill configurations.
 */
UCLASS(BlueprintType)
class USkillConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	/** Montage played when this ability activates (for example, fire animation). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability|Animation")
	TObjectPtr<UAnimMontage> FireMontage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability|Info")
	int32 AbilityIndex = -1;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bShouldEndAbilityOnFireMontageCompleted = true;
};

UCLASS(BlueprintType)
class UMeleeConfig : public USkillConfig
{
	GENERATED_BODY()

public:
	
};

UCLASS(BlueprintType)
class UHealthRegenConfig : public USkillConfig
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameplayEffect")
	TSubclassOf<UGameplayEffect> HealthRegenEffect;
};

UCLASS(BlueprintType)
class UDashConfig : public USkillConfig
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ForwardImpulse = 1000.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameplayEffect")
	TSubclassOf<UGameplayEffect> DashDamageEffect;
};

UCLASS(BlueprintType)
class ULaserConfig : public USkillConfig
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> LaserDamageEffect;
	
	UPROPERTY(editAnywhere, BlueprintReadOnly)
	TSubclassOf<ALaserActor> LaserActorClass;
	
	UPROPERTY(editAnywhere, BlueprintReadOnly)
	FGameplayTag LaserCostTag;
};

