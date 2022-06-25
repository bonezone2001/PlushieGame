// Fill out your copyright notice in the Description page of Project Settings.


#include "PlushieGameInfo.h"
#include "Components/PanelWidget.h"
#include "Net/UnrealNetwork.h"
#include "OnlineSubsystemUtils.h"
#include "Kismet/GameplayStatics.h"

void UPlushieGameInfo::ShowMenu(TSubclassOf<UUserWidget> MenuWidgetClass, UUserWidget*& Widget) const
{
	// Get player controller
	APlayerController* PlayerController = GetFirstLocalPlayerController();

	// Prevent crash if forgot to set
	if (!IsValid(MenuWidgetClass))
	{
		UE_LOG(LogTemp, Error, TEXT("MenuWidgetClass is not valid"));
		return;
	}

	// Create menu widget
	if (!IsValid(Widget))
	{
		Widget = CreateWidget<UUserWidget>(GetWorld(), MenuWidgetClass);
		Widget->SetOwningPlayer(PlayerController);
	}

	// Add menu widget to viewport
	Widget->AddToViewport();
	PlayerController->SetShowMouseCursor(true);
}

void UPlushieGameInfo::Init()
{
	Super::Init();

	GetEngine()->OnNetworkFailure().AddUObject(this, &UPlushieGameInfo::HandleNetworkFailure);
	GetEngine()->OnTravelFailure().AddUObject(this, &UPlushieGameInfo::HandleTravelFailure);
}

UPlushieGameInfo::UPlushieGameInfo()
{
	OnCreateSessionCompleteDelegate = FOnCreateSessionCompleteDelegate::CreateUObject(this, &UPlushieGameInfo::OnCreateSessionComplete);
	OnJoinSessionCompleteDelegate = FOnJoinSessionCompleteDelegate::CreateUObject(this, &UPlushieGameInfo::OnJoinSessionComplete);
	OnFindSessionsCompleteDelegate = FOnFindSessionsCompleteDelegate::CreateUObject(this, &UPlushieGameInfo::OnFindSessionsComplete);
}

bool UPlushieGameInfo::LaunchLobbyEx(FUniqueNetIdRepl UserId, FName SessionName, FString MapName,
                                   bool IsPresence, int32 MaxNumPlayers)
{
	// Show that we're doing something at least
	ShowLoadingScreen();

	// Get the Online Subsystem to work with
	IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();
	if (!OnlineSub)
	{
		UE_LOG(LogOnline, Warning, TEXT("No OnlineSubsystem found."));
		return false;
	}

	// Get the Session Interface, so we can call the "CreateSession" function on it
	const IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

	// Setup session properties
	SessionSettings = MakeShareable(new FOnlineSessionSettings());

	SessionSettings->bIsLANMatch = false;
	SessionSettings->bUsesPresence = IsPresence;
	SessionSettings->NumPublicConnections = MaxNumPlayers;
	SessionSettings->NumPrivateConnections = 0;
	SessionSettings->bAllowInvites = true;
	SessionSettings->bAllowJoinInProgress = true;
	SessionSettings->bShouldAdvertise = true;
	SessionSettings->bAllowJoinViaPresence = IsPresence;
	SessionSettings->bAllowJoinViaPresenceFriendsOnly = true;

	SessionSettings->Set(SETTING_MAPNAME, MapName, EOnlineDataAdvertisementType::ViaOnlineService);

	// Setup delegate to handle session creation/failure
	OnCreateSessionCompleteDelegateHandle = Sessions->AddOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegate);

	// Setup server settings struct
	ServerSettings.Name = SessionName;
	ServerSettings.MaxPlayers = MaxNumPlayers;
	ServerSettings.CurrentPlayers = 0;
	ServerSettings.Map = MapName;
	ServerSettings.Password = "";

	// Attempt to start the session
	return Sessions->CreateSession(*UserId, SessionName, *SessionSettings);
}

bool UPlushieGameInfo::JoinLobbyEx(FUniqueNetIdRepl UserId, FName SessionName,
                                 const FOnlineSessionSearchResult& SearchResult)
{
	// Get OnlineSubsystem we want to work with
	const IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (!OnlineSub)
	{
		UE_LOG(LogOnline, Warning, TEXT("No OnlineSubsystem found."));
		return false;
	}

	// Get SessionInterface from the OnlineSubsystem
	const IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
	if (!Sessions.IsValid() || !UserId.IsValid())
	{
		UE_LOG(LogOnline, Warning, TEXT("Invalid session interface or user id"));
		return false;
	}

	// Join session
	OnJoinSessionCompleteDelegateHandle = Sessions->AddOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegate);
	return Sessions->JoinSession(*UserId, SessionName, SearchResult);
}

bool UPlushieGameInfo::FindSessions(FUniqueNetIdRepl UserId, bool bIsPresence)
{
	// Get OnlineSubsystem we want to work with
	const IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (!OnlineSub)
	{
		UE_LOG(LogOnline, Warning, TEXT("No OnlineSubsystem found."));
		return false;
	}

	// Get SessionInterface from the OnlineSubsystem
	const IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
	if (!Sessions.IsValid() || !UserId.IsValid())
	{
		UE_LOG(LogOnline, Warning, TEXT("Invalid session interface or user id"));
		OnFindSessionsComplete(false);
		return false;
	}

	// Fill Search Settings
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->bIsLanQuery = false;
	SessionSearch->MaxSearchResults = 20;
	SessionSearch->PingBucketSize = 50;
	if (bIsPresence)
		SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, bIsPresence, EOnlineComparisonOp::Equals);

	// Setup handler and begin searching
	TSharedRef<FOnlineSessionSearch> SearchSettingsRef = SessionSearch.ToSharedRef();
	OnFindSessionsCompleteDelegateHandle = Sessions->AddOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegate);
	return Sessions->FindSessions(*UserId, SearchSettingsRef);
}

bool UPlushieGameInfo::LaunchLobby(const FName SessionName, const FString MapName, const bool IsPresence,
                                   const int32 MaxNumPlayers)
{
	ULocalPlayer* const Player = GetFirstGamePlayer();
	return LaunchLobbyEx(Player->GetPreferredUniqueNetId(), SessionName, MapName, IsPresence, MaxNumPlayers);
}

bool UPlushieGameInfo::JoinLobby(FName SessionName, FBlueprintSessionResult SearchResult)
{
	ULocalPlayer* const Player = GetFirstGamePlayer();
	return JoinLobbyEx(Player->GetPreferredUniqueNetId(), SessionName, SearchResult.OnlineResult);
}

bool UPlushieGameInfo::DestroySession(const FName SessionName)
{
	// Get OnlineSubsystem we want to work with
	const IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (!OnlineSub)
	{
		UE_LOG(LogOnline, Warning, TEXT("No OnlineSubsystem found."));
		return false;
	}

	// Get SessionInterface from the OnlineSubsystem
	const IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
	if (!Sessions.IsValid())
	{
		UE_LOG(LogOnline, Warning, TEXT("Invalid session interface or user id"));
		return false;
	}

	return Sessions->DestroySession(SessionName);
}

void UPlushieGameInfo::HandleTravelFailure(UWorld* World, ETravelFailure::Type FailureType, const FString& ErrorMessage) const
{
	DisplayErrorMessage(ErrorMessage);
}

FString UPlushieGameInfo::NetworkFailureToString() const
{
	return "";
}

void UPlushieGameInfo::HandleNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType,
                                            const FString& ErrorString) const
{
	DisplayErrorMessage(ErrorString);
	UE_LOG(LogTemp, Error, TEXT("Network Failure: %s"), *ErrorString);
}

void UPlushieGameInfo::ShowMainMenu()
{
	ShowMenu(MainMenuWidgetClass, MainMenuWidget);
	EventShowMainMenu();
}

void UPlushieGameInfo::ShowHostMenu()
{
	ShowMenu(HostMenuWidgetClass, HostMenuWidget);
	EventShowHostMenu();
}

void UPlushieGameInfo::ShowOptionsMenu()
{
	ShowMenu(OptionsMenuWidgetClass, OptionsMenuWidget);
	EventShowOptionsMenu();
}

void UPlushieGameInfo::ShowServerMenu()
{
	UUserWidget* Widget = nullptr;
	ShowMenu(ServerMenuWidgetClass, Widget);
	EventShowHostMenu();
}

void UPlushieGameInfo::ShowLoadingScreen()
{
}

void UPlushieGameInfo::DisplayErrorMessage(const FString& Message) const
{
}

void UPlushieGameInfo::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UPlushieGameInfo, ServerSettings);
}

void UPlushieGameInfo::OnCreateSessionComplete(const FName SessionName, const bool WasSuccessful)
{
	UE_LOG(LogOnline, Log, TEXT("OnCreateSessionComplete %s bSuccess: %d"), *SessionName.ToString(), WasSuccessful);

	// Handle failure
	if (!WasSuccessful)
	{
		DisplayErrorMessage("We Were Unable to Create a Session. Please Try Again.");
		UE_LOG(LogOnline, Warning, TEXT("Failed to create session %s"), *SessionName.ToString());
		return;
	}

	// Load Lobby Level
	UGameplayStatics::OpenLevel(GetWorld(), "Lobby", true, "listen");

	// Send event
	EventOnCreateSessionComplete(SessionName, WasSuccessful);
}

void UPlushieGameInfo::OnJoinSessionComplete(const FName SessionName, const EOnJoinSessionCompleteResult::Type Result)
{
	if (Result != EOnJoinSessionCompleteResult::Success)
	{
		DisplayErrorMessage("We Were Unable to Join the Session. Please Try Again.");
		UE_LOG(LogOnline, Warning, TEXT("Failed to join session %s"), *SessionName.ToString());

		// Here so I don't accidentally make 100 bugs in the future
		return;
	}

	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, TEXT("Successfully Joined Session"));

	// Send event
	EventOnJoinSessionComplete(SessionName, Result);
}

void UPlushieGameInfo::OnFindSessionsComplete(bool WasSuccessful)
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("OFindSessionsComplete bSuccess: %d"), WasSuccessful));

	// Get OnlineSubsystem we want to work with
	const IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (!OnlineSub)
	{
		UE_LOG(LogOnline, Warning, TEXT("No OnlineSubsystem found."));
		return;
	}

	// Get SessionInterface from the OnlineSubsystem
	const IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
	if (!Sessions.IsValid())
	{
		UE_LOG(LogOnline, Warning, TEXT("Invalid session interface or user id"));
		return;
	}

	// Clear the Delegate handle, since we finished this call
	Sessions->ClearOnFindSessionsCompleteDelegate_Handle(const_cast<FDelegateHandle&>(OnFindSessionsCompleteDelegateHandle));

	// Just debugging the Number of Search results. Can be displayed in UMG or something later on
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Num Search Results: %d"), SessionSearch->SearchResults.Num()));

	// If we have found at least 1 session, we just going to debug them. You could add them to a list of UMG Widgets, like it is done in the BP version!
	if (!SessionSearch->SearchResults.Num())
		{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("No Search Results")));
		return;
	}
		
	// "SessionSearch->SearchResults" is an Array that contains all the information. You can access the Session in this and get a lot of information.
	// This can be customized later on with your own classes to add more information that can be set and displayed
	for (int32 SearchIdx = 0; SearchIdx < SessionSearch->SearchResults.Num(); SearchIdx++)
	{
		// OwningUserName is just the SessionName for now. I guess you can create your own Host Settings class and GameSession Class and add a proper GameServer Name here.
		// This is something you can't do in Blueprint for example!
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Session Number: %d | Sessionname: %s "), SearchIdx + 1, *(SessionSearch->SearchResults[SearchIdx].Session.OwningUserName)));
	}

	// Convert from FOnlineSessionSearchResult array to FBlueprintSessionResult array
	TArray<FBlueprintSessionResult> BlueprintSessionResults;
	for (const auto& SearchResult : SessionSearch->SearchResults)
	{
		FBlueprintSessionResult BlueprintSessionResult;
		BlueprintSessionResult.OnlineResult = SearchResult;
		BlueprintSessionResults.Add(BlueprintSessionResult);
	}

	// Send event
	EventOnFindSessionsComplete(WasSuccessful, BlueprintSessionResults);
}
