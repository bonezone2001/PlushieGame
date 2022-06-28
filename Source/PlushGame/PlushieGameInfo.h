// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "ServerSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include <OnlineSubsystemUtils/Classes/FindSessionsCallbackProxy.h>
#include "PlushieGameInfo.generated.h"

// I know FOnSessionsFound is basically inside of FindSessionsCallbackProxy but I want to pass success/failure as well
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSessionsFound, const TArray<FBlueprintSessionResult>&, Results, bool, WasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSessionCreate, FName, SessionName, bool, WasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSessionJoin, FName, SessionName, int, ReturnCode);

// Handles the UI and lobby join/create (currently)
// TODO: Abstract out internals to a separate classes
UCLASS()
class PLUSHGAME_API UPlushieGameInfo : public UGameInstance
{
	GENERATED_BODY()

	IOnlineSessionPtr Sessions;
	TSharedPtr<FOnlineSessionSearch> SessionSearch;

	UPROPERTY(VisibleAnywhere, Replicated)
	FServerSettings ServerSettings;

public:
	UPROPERTY(EditDefaultsOnly, Category = UI)
	TSubclassOf<UUserWidget> MainMenuWidgetClass;
	
	UPROPERTY()
	UUserWidget* MainMenuWidget;

	UPROPERTY(EditDefaultsOnly, Category = UI)
	TSubclassOf<UUserWidget> HostMenuWidgetClass;

	UPROPERTY()
	UUserWidget* HostMenuWidget;

	UPROPERTY(EditDefaultsOnly, Category = UI)
	TSubclassOf<UUserWidget> OptionsMenuWidgetClass;

	UPROPERTY()
	UUserWidget* OptionsMenuWidget;
	
	UPROPERTY(EditDefaultsOnly, Category=UI)
	TSubclassOf<UUserWidget> ServerMenuWidgetClass;

	UPROPERTY(BlueprintAssignable, Category=EventDispatchers)
	FOnSessionsFound OnSessionsFoundDelegate;

	UPROPERTY(BlueprintAssignable, Category=EventDispatchers)
	FOnSessionCreate OnSessionCreatedDelegate;

	UPROPERTY(BlueprintAssignable, Category=EventDispatchers)
	FOnSessionJoin OnSessionJoinedDelegate;

private:
	void ShowMenu(TSubclassOf<UUserWidget> MenuWidgetClass, UUserWidget*& Widget) const;

protected:
	virtual void Init() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	/*
	 * World
	 */
	void HandleTravelFailure(UWorld* World, ETravelFailure::Type FailureType, const FString& ErrorMessage) const;
	
	/*
	 * UI functionality and events
	 */
	UFUNCTION(BlueprintCallable, Category=Game)
	void ShowMainMenu();

	UFUNCTION(BlueprintCallable, Category=Game)
	void ShowHostMenu();

	UFUNCTION(BlueprintCallable, Category=Game)
	void ShowOptionsMenu();

	UFUNCTION(BlueprintCallable, Category=Game)
	void ShowServerMenu();

	UFUNCTION(BlueprintCallable, Category = Game)
	void DisplayErrorMessage(const FString& Message) const;

	UFUNCTION(BlueprintImplementableEvent, Category = Game)
	void EventErrorMessage(const FString& Message);

	/*
	 * Networking
	 */
	void HandleNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString) const;

	virtual void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	virtual void OnFindSessionsComplete(bool bWasSuccessful);
	virtual void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	UFUNCTION(BlueprintCallable, Category=Networking)
	bool LaunchLobby(const FName SessionName, const FString MapName, const int32 MaxNumPlayers);

	bool JoinLobbyEx(FName SessionName, const FOnlineSessionSearchResult& SearchResult);
	UFUNCTION(BlueprintCallable, Category=Networking)
	bool JoinLobby(FName SessionName, const FBlueprintSessionResult& SearchResult);

	UFUNCTION(BlueprintCallable, Category=Networking)
	bool FindSessions();

	UFUNCTION(BlueprintCallable, Category=Networking)
	bool DestroySession(FName SessionName) const;
};
