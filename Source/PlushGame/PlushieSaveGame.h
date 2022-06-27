// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerInfo.h"
#include "GameFramework/SaveGame.h"
#include "PlushieSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class PLUSHGAME_API UPlushieSaveGame : public USaveGame
{
	GENERATED_BODY()

public: 
	UPROPERTY(EditDefaultsOnly, Replicated, Category = "SaveGame")
	FPlayerInfo PlayerInfo;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
