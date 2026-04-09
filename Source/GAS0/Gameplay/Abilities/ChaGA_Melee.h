// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GAS0CharacterGameplayAbility.h"
#include "ChaGA_Melee.generated.h"

/**
 * Melee ability that randomly chooses montage start section "1" or "2".
 */
UCLASS()
class UChaGA_Melee : public UGAS0CharacterGameplayAbility
{
    GENERATED_BODY()

protected:
    virtual bool PlayFireMontage() override;
};

