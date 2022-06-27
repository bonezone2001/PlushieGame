// Fill out your copyright notice in the Description page of Project Settings.


#include "SteamBPFL.h"

// This is a mouthful
#include "ThirdParty/Steamworks/Steamv151/sdk/public/steam/steam_api.h"

FString USteamBPFL::GetStringSteamID()
{
	FString SteamID;

	// TODO: Temporary log and forget
	if (!SteamAPI_Init())
	{
		UE_LOG(LogTemp, Error, TEXT("Steam not initialized, returning 0"));
		return FString("0");
	}

	CSteamID steamID = SteamUser()->GetSteamID();
	return FString::FromInt(steamID.ConvertToUint64());
}
