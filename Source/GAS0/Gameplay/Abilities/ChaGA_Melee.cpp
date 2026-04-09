// Copyright Epic Games, Inc. All Rights Reserved.

#include "ChaGA_Melee.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

bool UChaGA_Melee::PlayFireMontage()
{
    if (!FireMontage)
    {
        return false;
    }

    // Build a seeded random stream and choose start section 1 or 2.
    const int32 Seed = FMath::Rand();
    FRandomStream RandomStream(Seed);
    const int32 SectionValue = RandomStream.RandRange(1, 2);
    const FName StartSection(*FString::FromInt(SectionValue));

    ActiveMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, FireMontage, 1.0f, StartSection);
    if (!ActiveMontageTask)
    {
        return false;
    }

    ActiveMontageTask->OnCompleted.AddDynamic(this, &UChaGA_Melee::OnFireMontageCompleted);
    ActiveMontageTask->OnBlendOut.AddDynamic(this, &UChaGA_Melee::OnFireMontageBlendOut);
    ActiveMontageTask->OnInterrupted.AddDynamic(this, &UChaGA_Melee::OnFireMontageInterrupted);
    ActiveMontageTask->OnCancelled.AddDynamic(this, &UChaGA_Melee::OnFireMontageCancelled);
    ActiveMontageTask->ReadyForActivation();

    return true;
}

