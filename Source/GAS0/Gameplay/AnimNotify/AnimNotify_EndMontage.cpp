// Fill out your copyright notice in the Description page of Project Settings.

#include "Gameplay/AnimNotify/AnimNotify_EndMontage.h"
#include "Gameplay/AnimInstance/AnimInst_Shinbi.h"

void UAnimNotify_EndMontage::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	if (MeshComp)
	{
		if (UAnimInst_Shinbi* ShinbiAnimInst = Cast<UAnimInst_Shinbi>(MeshComp->GetAnimInstance()))
		{
			ShinbiAnimInst->OnEndMontageNotify();
		}
	}
}