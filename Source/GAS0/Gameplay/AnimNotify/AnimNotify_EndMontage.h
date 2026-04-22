// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_EndMontage.generated.h"

/**
 * 
 */
UCLASS()
class GAS0_API UAnimNotify_EndMontage : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	/** Perform the Anim Notify */
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
