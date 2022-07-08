#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "MasterGM.generated.h"

/**
 * Parent class for all my game modes.
 */
UCLASS()
class PLUSHGAME_API AMasterGM : public AGameMode
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	TArray<class AMasterPC*> Players;

	UPROPERTY()
	TArray<AActor*> PlayerStarts;
	
	UPROPERTY()
	int32 NeededPlayers;

	// Countdown
	UPROPERTY(EditDefaultsOnly)
	bool bUsesCountdown;
	
	UPROPERTY(EditDefaultsOnly)
	int32 CountdownTime;

	UPROPERTY()
	FTimerHandle CountdownTimerHandle;
	
public:
	AMasterGM();

protected:
	// Track players in the game.
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;

	// Match state
	virtual bool IsReadyStartMatch();

	virtual void Tick(float DeltaSeconds) override;
	// override choose player start ue4
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

public:
	UFUNCTION(BlueprintCallable)
	virtual void StartCountdown();

	UFUNCTION(BlueprintCallable)
	virtual void EndCountdown();
};