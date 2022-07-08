#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MasterPC.generated.h"

// Define one param delegate
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCountdownStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCountdownEnded);

/**
 * Parent class for all my player controllers.
 */
UCLASS()
class PLUSHGAME_API AMasterPC : public APlayerController
{
	GENERATED_BODY()

	// Components
	UPROPERTY(EditDefaultsOnly, BlueprintGetter=GetUI, Category=UI)
	class UUIComponent* UIComponent;

	UPROPERTY(BlueprintAssignable, Category=EventDispatchers)
	FOnCountdownStarted OnCountdownStarted;

	UPROPERTY(BlueprintAssignable, Category=EventDispatchers)
	FOnCountdownEnded OnCountdownEnded;
	
public:
	AMasterPC(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	// Getters
	UFUNCTION(BlueprintGetter)
	class UUIComponent* GetUI() const;

	UFUNCTION(Client, Reliable)
	void CountdownStarted();

	UFUNCTION(Client, Reliable)
	void CountdownEnded();
};
