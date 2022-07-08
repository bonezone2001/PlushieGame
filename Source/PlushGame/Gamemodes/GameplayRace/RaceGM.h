// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlushGame/Gamemodes/Base/MasterGM.h"
#include "RaceGM.generated.h"

/**
 * 
 */
UCLASS()
class PLUSHGAME_API ARaceGM : public AMasterGM
{
	GENERATED_BODY()

public:
	virtual void EndCountdown() override;
};
