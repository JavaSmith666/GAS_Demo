// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GAS0GameMode.generated.h"

/**
 *  Simple GameMode for a third person game
 */
UCLASS(abstract)
class AGAS0GameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	
	/** Constructor */
	AGAS0GameMode();
};



