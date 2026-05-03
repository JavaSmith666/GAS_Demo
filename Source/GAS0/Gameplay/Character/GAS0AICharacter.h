// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/Character/GAS0Character.h"
#include "GAS0AICharacter.generated.h"

/**
 * 
 */
UCLASS()
class AGAS0AICharacter : public AGAS0Character
{
	GENERATED_BODY()
	
public:
	AGAS0AICharacter();
	
	virtual void BeginPlay() override;
};
