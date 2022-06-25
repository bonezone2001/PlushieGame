// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerInfo.generated.h"

USTRUCT(BlueprintType)
struct FPlayerInfo
{
	GENERATED_BODY()

	// Just what I know we need now
	FString Name;
	FString Status;

	// Will add more later because we'll need to store cosmetics and other player specific information in here
};
