#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "MasterGS.generated.h"

/**
 * Data that all clients should know about regardless of game mode
 */
UCLASS()
class PLUSHGAME_API AMasterGS : public AGameState
{
	GENERATED_BODY()

public:
	AMasterGS();
	
public:
	UPROPERTY(ReplicatedUsing=OnRep_Countdown, BlueprintReadOnly)
	int32 Countdown;

	UPROPERTY(ReplicatedUsing=OnRep_Round, BlueprintReadOnly)
	int32 Round;

	UFUNCTION()
	virtual void OnRep_Countdown();
	
	UFUNCTION()
	virtual void OnRep_Round();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
