// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "ServerSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include <OnlineSubsystemUtils/Classes/FindSessionsCallbackProxy.h>
#include "PlushieGameInfo.generated.h"

/**
 * Handles the UI and lobby join/create (currently)
 */
UCLASS()
class PLUSHGAME_API UPlushieGameInfo : public UGameInstance
{
	GENERATED_BODY()

	FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate;
	FDelegateHandle OnCreateSessionCompleteDelegateHandle;

	FOnJoinSessionCompleteDelegate OnJoinSessionCompleteDelegate;
	FDelegateHandle OnJoinSessionCompleteDelegateHandle;

	TSharedPtr<class FOnlineSessionSearch> SessionSearch;
	FOnFindSessionsCompleteDelegate OnFindSessionsCompleteDelegate;
	FDelegateHandle OnFindSessionsCompleteDelegateHandle;

	TSharedPtr<FOnlineSessionSettings> SessionSettings;

	UPROPERTY(VisibleAnywhere, Replicated)
	FServerSettings ServerSettings;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> MainMenuWidgetClass;

	UPROPERTY()
	UUserWidget* MainMenuWidget;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> HostMenuWidgetClass;

	UPROPERTY()
	UUserWidget* HostMenuWidget;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> OptionsMenuWidgetClass;

	UPROPERTY()
	UUserWidget* OptionsMenuWidget;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> LoadingScreenWidgetClass;

	UPROPERTY()
	UUserWidget* LoadingScreenWidget;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> ServerMenuWidgetClass;

	void ShowMenu(TSubclassOf<UUserWidget> MenuWidgetClass, UUserWidget*& Widget) const;

protected:
	virtual void Init() override;

public:
	UPlushieGameInfo();

	/*
	 * World
	 */
	void HandleTravelFailure(UWorld* World, ETravelFailure::Type FailureType, const FString& ErrorMessage) const;
	
	/*
	 * UI functionality and events
	 */
	UFUNCTION(BlueprintCallable, Category=Game)
	void ShowMainMenu();

	UFUNCTION(BlueprintImplementableEvent, Category=Game)
	void EventShowMainMenu();

	UFUNCTION(BlueprintCallable, Category=Game)
	void ShowHostMenu();

	UFUNCTION(BlueprintImplementableEvent, Category=Game)
	void EventShowHostMenu();

	UFUNCTION(BlueprintCallable, Category=Game)
	void ShowOptionsMenu();

	UFUNCTION(BlueprintImplementableEvent, Category=Game)
	void EventShowOptionsMenu();

	UFUNCTION(BlueprintCallable, Category=Game)
	void ShowServerMenu();

	UFUNCTION(BlueprintImplementableEvent, Category=Game)
	void EventShowServerMenu();

	UFUNCTION(BlueprintCallable, Category=Game)
	static void ShowLoadingScreen();

	UFUNCTION(BlueprintImplementableEvent, Category=Game)
	void EventShowLoadingScreen();

	UFUNCTION(BlueprintCallable, Category = Game)
	void DisplayErrorMessage(const FString& Message) const;

	UFUNCTION(BlueprintImplementableEvent, Category = Game)
	void EventErrorMessage(const FString& Message);

	/*
	 * Networking
	 */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Cannot make these into UFUNCTIONs due to FUniqueNetId not being a base type, this is just wrapped below
	bool LaunchLobbyEx(FUniqueNetIdRepl UserId, FName SessionName, FString MapName, bool IsPresence, int32 MaxNumPlayers);
	bool JoinLobbyEx(FUniqueNetIdRepl UserId, FName SessionName, const FOnlineSessionSearchResult& SearchResult);
	bool FindSessions(FUniqueNetIdRepl UserId, bool bIsPresence);


	UFUNCTION(BlueprintCallable, Category=Networking)
	bool LaunchLobby(const FName SessionName, const FString MapName, const bool IsPresence, const int32 MaxNumPlayers);
	
	UFUNCTION(BlueprintCallable, Category=Networking)
	bool JoinLobby(FName SessionName, FBlueprintSessionResult SearchResult);

	UFUNCTION(BlueprintCallable, Category=Networking)
	bool DestroySession(const FName SessionName);

	FString NetworkFailureToString() const;
	void HandleNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString) const;

	void OnCreateSessionComplete(const FName SessionName, const bool WasSuccessful);
	UFUNCTION(BlueprintImplementableEvent, Category=Network)
	void EventOnCreateSessionComplete(FName SessionName, bool WasSuccessful);

	void OnJoinSessionComplete(const FName SessionName, const EOnJoinSessionCompleteResult::Type Result);
	UFUNCTION(BlueprintImplementableEvent, Category=Network)
	void EventOnJoinSessionComplete(FName SessionName, int Type);

	void OnFindSessionsComplete(bool WasSuccessful);
	UFUNCTION(BlueprintImplementableEvent, Category=Network)
	void EventOnFindSessionsComplete(bool WasSuccessful, const TArray<FBlueprintSessionResult>& Sessions);
};
