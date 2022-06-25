 // Fill out your copyright notice in the Description page of Project Settings.


#include "PlushieSaveGame.h"
#include "Net/UnrealNetwork.h"

void UPlushieSaveGame::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
 	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    // Replicate to everyone
    DOREPLIFETIME(UPlushieSaveGame, PlayerInfo);
}
