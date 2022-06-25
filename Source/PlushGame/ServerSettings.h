// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ServerSettings.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FServerSettings
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FName Name;
		
	UPROPERTY(BlueprintReadWrite)
	FString Password;
		
	UPROPERTY(BlueprintReadWrite)
	FString Map;
		
	UPROPERTY(BlueprintReadWrite)
	int MaxPlayers;
		
	UPROPERTY(BlueprintReadWrite)
	int CurrentPlayers;
};