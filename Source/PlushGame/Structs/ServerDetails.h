#pragma once

#include "Interfaces/OnlineSessionInterface.h"
#include <OnlineSubsystemUtils/Classes/FindSessionsCallbackProxy.h>
#include "ServerDetails.generated.h"

/*
 * Details of server used by server and clients.
 */
USTRUCT(BlueprintType)
struct FServerDetails
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString Name;

	UPROPERTY(BlueprintReadOnly)
	FString Owner;

	UPROPERTY(BlueprintReadOnly)
	FString Map;

	UPROPERTY(BlueprintReadOnly)
	FBlueprintSessionResult Session;
};
