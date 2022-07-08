#include "MasterGM.h"

#include "EngineUtils.h"
#include "MasterGS.h"
#include "MasterPC.h"
#include "Engine/PlayerStartPIE.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"

AMasterGM::AMasterGM()
{
	PrimaryActorTick.bCanEverTick = true;
	bPauseable = false;
	//bDelayedStart = true;

	// Defaults
	bUsesCountdown = false;
	CountdownTime = 10;
	NeededPlayers = 1;
}

void AMasterGM::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	Players.Add(static_cast<AMasterPC*>(NewPlayer));
	UE_LOG(LogTemp, Log, TEXT("%s has joined the game"), *NewPlayer->GetName());
	//Say(TEXT("PlayerJoined")); is put into console (press tilde twice)

	if (bUsesCountdown && Players.Num() >= NeededPlayers)
		StartCountdown();
}

void AMasterGM::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	Players.Remove(static_cast<AMasterPC*>(Exiting));
}

bool AMasterGM::IsReadyStartMatch()
{
	// TODO: Add system for preventing game start until all players are ready
	return true;
}

void AMasterGM::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

AActor* AMasterGM::ChoosePlayerStart_Implementation(AController* Player)
{
	// Reset the array when ran out
	if (PlayerStarts.Num() == 0)
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStarts);
	
	AActor* FoundPlayerStart = PlayerStarts.Num() == 0 ? nullptr : PlayerStarts[0];
	if (FoundPlayerStart)
		PlayerStarts.RemoveAt(0);
	return FoundPlayerStart;
}

void AMasterGM::StartCountdown()
{
	if (GetWorldTimerManager().IsTimerActive(CountdownTimerHandle))
		return;

	// Setup countdown timer
	GetGameState<AMasterGS>()->Countdown = CountdownTime;
	GetWorldTimerManager().SetTimer(CountdownTimerHandle, [&]{
		auto& Countdown = GetGameState<AMasterGS>()->Countdown;
		Countdown--;
		if (Countdown <= 0)
			EndCountdown();
	}, 1.0f, true);

	// Tell everyone that the countdown has started
	for (auto& Player : Players)
		Player->CountdownStarted();
}

void AMasterGM::EndCountdown()
{
	GetWorldTimerManager().ClearTimer(CountdownTimerHandle);
	GetGameState<AMasterGS>()->Countdown = 0;
	
	// Tell everyone that the countdown has ended
	for (const auto& Player : Players)
		Player->CountdownEnded();
}