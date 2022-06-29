// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Interfaces/OnlineSessionInterface.h"
#include <OnlineSubsystemUtils/Classes/FindSessionsCallbackProxy.h>
#include "ServerSettings.generated.h"

// Server settings replicated to all clients
// Not the same as session settings
USTRUCT(BlueprintType)
struct FServerSettings
{
	GENERATED_BODY()
		
	UPROPERTY(BlueprintReadOnly)
	FString Name;
	
	UPROPERTY(BlueprintReadOnly)
	FString Owner;
		
	UPROPERTY(BlueprintReadOnly)
	FString Password;
		
	UPROPERTY(BlueprintReadOnly)
	FString Map;

	UPROPERTY(BlueprintReadOnly)
	FBlueprintSessionResult Session;
};
