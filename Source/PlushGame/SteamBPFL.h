// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SteamBPFL.generated.h"

// Handling all steam related functionality
UCLASS()
class PLUSHGAME_API USteamBPFL : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category=Steam)
	static FString GetStringSteamID();

	
};
