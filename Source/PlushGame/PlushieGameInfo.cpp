// Fill out your copyright notice in the Description page of Project Settings.
// File needs to be refactored at some point

#include "PlushieGameInfo.h"
#include "Components/PanelWidget.h"
#include "Net/UnrealNetwork.h"
#include "OnlineSubsystemUtils.h"
#include "Kismet/GameplayStatics.h"

void UPlushieGameInfo::Init()
{
	Super::Init();

	OnlineSubsystem = IOnlineSubsystem::Get();
	check(OnlineSubsystem)

	SessionInterface = OnlineSubsystem->GetSessionInterface();
	check(SessionInterface.IsValid())

	// Delegates
	GetEngine()->OnNetworkFailure().AddUObject(this, &UPlushieGameInfo::HandleNetworkFailure);
	GetEngine()->OnTravelFailure().AddUObject(this, &UPlushieGameInfo::HandleTravelFailure);

	SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UPlushieGameInfo::OnCreateSessionComplete);
	SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UPlushieGameInfo::OnFindSessionsComplete);
	SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UPlushieGameInfo::OnJoinSessionComplete);
}

void UPlushieGameInfo::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void UPlushieGameInfo::DisplayErrorMessage(const FString& Message) const
{
	// TODO: Implement
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, Message);
}

void UPlushieGameInfo::ShowMenu(TSubclassOf<UUserWidget> MenuWidgetClass, UUserWidget*& Widget) const
{
	APlayerController* Player = GetFirstLocalPlayerController();

	// Check if we provided a widget class
	if (!IsValid(MenuWidgetClass))
	{
		UE_LOG(LogTemp, Error, TEXT("MenuWidgetClass is not valid"));
		return;
	}

	// Create widget
	if (!IsValid(Widget))
	{
		Widget = CreateWidget<UUserWidget>(GetWorld(), MenuWidgetClass);
		Widget->SetOwningPlayer(Player);
	}

	Widget->AddToViewport();
	Player->SetShowMouseCursor(true);
}

void UPlushieGameInfo::ShowMainMenu()
{
	ShowMenu(MainMenuWidgetClass, MainMenuWidget);
}

void UPlushieGameInfo::ShowHostMenu()
{
	ShowMenu(HostMenuWidgetClass, HostMenuWidget);
}

void UPlushieGameInfo::ShowOptionsMenu()
{
	ShowMenu(OptionsMenuWidgetClass, OptionsMenuWidget);
}

void UPlushieGameInfo::ShowServerMenu()
{
	UUserWidget* Widget = nullptr;
	ShowMenu(ServerMenuWidgetClass, Widget);
}

bool UPlushieGameInfo::LaunchLobby(FName SessionName, FString MapName, int32 MaxNumPlayers)
{
	UE_LOG(LogTemp, Warning, TEXT("Launching online session..."))

	// Session settings used by UE
	FOnlineSessionSettings SessionSettings;
	SessionSettings.bAllowJoinInProgress = true;
	SessionSettings.bIsDedicated = false;
	SessionSettings.bIsLANMatch = true;
	SessionSettings.bUsesPresence = false;
	if (OnlineSubsystem->GetSubsystemName() == "Steam")
	{
		SessionSettings.bUsesPresence = true;
		SessionSettings.bIsLANMatch = false;
	}
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.bUseLobbiesIfAvailable = true;
	SessionSettings.NumPublicConnections = MaxNumPlayers;

	// Server settings
	ServerSettings.Name = SessionName.ToString();
	ServerSettings.Map = MapName;
	ServerSettings.Password = "";

	SessionSettings.Set("SERVER_NAME", SessionName.ToString(), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	SessionSettings.Set("SERVER_MAP", MapName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	return SessionInterface->CreateSession(0, SessionName, SessionSettings);
}

bool UPlushieGameInfo::JoinLobbyEx(FName SessionName, const FOnlineSessionSearchResult& SearchResult)
{
	return SessionInterface->JoinSession(0, SessionName, SearchResult);
}

bool UPlushieGameInfo::JoinLobby(FName SessionName, const FBlueprintSessionResult& SearchResult)
{
	return JoinLobbyEx(SessionName, SearchResult.OnlineResult);
}

bool UPlushieGameInfo::FindSessions()
{
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->bIsLanQuery = false;
	SessionSearch->MaxSearchResults = 10000;
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	return SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
}

bool UPlushieGameInfo::DestroySession(const FName SessionName) const
{
	return SessionInterface->DestroySession(SessionName);
}

void UPlushieGameInfo::OnCreateSessionComplete(const FName SessionName, const bool WasSuccessful)
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("OnCreateSessionComplete %s, %d"), *SessionName.ToString(), WasSuccessful));

	if (!WasSuccessful)
	{
		DisplayErrorMessage("We Were Unable to Create the Session. Please Try Again.");
		OnSessionCreatedDelegate.Broadcast(SessionName, WasSuccessful);
		return;
	}

	GetWorld()->ServerTravel("/Game/Levels/" + ServerSettings.Map + "?listen");
	// Note: marking session as "In progress" requires:
	// Sessions->StartSession(SessionName);

	// Broadcast for blueprints
	OnSessionCreatedDelegate.Broadcast(SessionName, WasSuccessful);
}

void UPlushieGameInfo::OnFindSessionsComplete(bool WasSuccessful)
{
	TArray<FServerSettings> FoundServers;
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("OFindSessionsComplete bSuccess: %d"), WasSuccessful));

	auto SearchResults = SessionSearch->SearchResults;
	UE_LOG(LogTemp, Warning, TEXT("Found %d sessions"), SearchResults.Num());
	if (SearchResults.Num() == 0)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("No Sessions Found")));
		OnSessionsFoundDelegate.Broadcast(FoundServers, WasSuccessful);
		return;
	}

	// Convert to blueprint session result
	for (const auto& SearchResult : SearchResults)
	{
		FServerSettings FoundSettings;

		FBlueprintSessionResult ConvertedResult;
		ConvertedResult.OnlineResult = SearchResult;
		FoundSettings.Session = ConvertedResult;

		FoundSettings.Owner = SearchResult.Session.OwningUserName;
		SearchResult.Session.SessionSettings.Get("SERVER_NAME", FoundSettings.Name);
		SearchResult.Session.SessionSettings.Get("SERVER_MAP", FoundSettings.Map);

		FoundServers.Add(FoundSettings);
	}

	// Broadcast for blueprints
	OnSessionsFoundDelegate.Broadcast(FoundServers, WasSuccessful);
}

void UPlushieGameInfo::OnJoinSessionComplete(const FName SessionName, const EOnJoinSessionCompleteResult::Type Result)
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("OnJoinSessionComplete %s, %d"), *SessionName.ToString(), Result));

	// TODO: Temporary log and forget
	if (Result != EOnJoinSessionCompleteResult::Success)
	{
		DisplayErrorMessage("We Were Unable to Join the Session. Please Try Again.");
		UE_LOG(LogOnline, Warning, TEXT("Failed to join session %s"), *SessionName.ToString());
		OnSessionJoinedDelegate.Broadcast(SessionName, Result);
		return;
	}

	// Begin travel to session map
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	FString ConnectString = "";
	SessionInterface->GetResolvedConnectString(SessionName, ConnectString);
	if (ConnectString.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to get connection information from client!"));
		OnSessionJoinedDelegate.Broadcast(SessionName, Result);
		return;
	}

	PlayerController->ClientTravel(ConnectString, ETravelType::TRAVEL_Absolute);

	// Broadcast for blueprints
	OnSessionJoinedDelegate.Broadcast(SessionName, Result);
}

void UPlushieGameInfo::HandleTravelFailure(UWorld* World, ETravelFailure::Type FailureType, const FString& ErrorMessage) const
{
	DisplayErrorMessage(ErrorMessage);
}

void UPlushieGameInfo::HandleNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString) const
{
	// TODO: Temporary log and forget
	DisplayErrorMessage(ErrorString);
	UE_LOG(LogTemp, Error, TEXT("Network Failure: %s"), *ErrorString);
}