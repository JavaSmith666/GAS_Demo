#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SkillConfig.generated.h"

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
};

UCLASS(BlueprintType)
class UMeleeConfig : public USkillConfig
{
	GENERATED_BODY()

public:
	
};
