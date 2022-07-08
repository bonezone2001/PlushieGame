#include "NetworkSubsystem.h"
#include "OnlineSubsystemUtils.h"

void UNetworkSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);;

	OnlineSubsystem = IOnlineSubsystem::Get();
	check(OnlineSubsystem != nullptr)

	SessionInterface = OnlineSubsystem->GetSessionInterface();
	check(SessionInterface.IsValid())

	check(GEngine)
	GEngine->OnNetworkFailure().AddUObject(this, &UNetworkSubsystem::HandleNetworkFailure);
	GEngine->OnTravelFailure().AddUObject(this, &UNetworkSubsystem::HandleTravelFailure);

	SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UNetworkSubsystem::OnCreateSessionComplete);
	SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UNetworkSubsystem::OnFindSessionsComplete);
	SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UNetworkSubsystem::OnJoinSessionComplete);
}

void UNetworkSubsystem::Deinitialize()
{
	Super::Deinitialize();

	// Delete delegates
	GEngine->OnNetworkFailure().RemoveAll(this);
	GEngine->OnTravelFailure().RemoveAll(this);

	SessionInterface->OnCreateSessionCompleteDelegates.RemoveAll(this);
	SessionInterface->OnFindSessionsCompleteDelegates.RemoveAll(this);
	SessionInterface->OnJoinSessionCompleteDelegates.RemoveAll(this);
}

void UNetworkSubsystem::HandleNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString)
{
	OnError(ErrorString);
}

void UNetworkSubsystem::HandleTravelFailure(UWorld* World, ETravelFailure::Type FailureType, const FString& ErrorString)
{
	OnError(ErrorString);
}

bool UNetworkSubsystem::LaunchLobby(const FString Name, const FString Map, const int32 MaxPlayers)
{
	// Setup session params
	SessionSettings.bAllowJoinInProgress = true;
	SessionSettings.bIsDedicated = false;
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.bUseLobbiesIfAvailable = true;
	SessionSettings.NumPublicConnections = MaxPlayers;

	// Switch based on subsystem
	if (OnlineSubsystem->GetSubsystemName() == "Steam")
	{
		SessionSettings.bIsLANMatch = false;
		SessionSettings.bUsesPresence = true;
	}
	else
	{
		SessionSettings.bIsLANMatch = true;
		SessionSettings.bUsesPresence = false;
	}

	// Advertisement settings
	SessionSettings.Set(SETTING_MAPNAME, Map, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	SessionSettings.Set(SETTING_SERVERNAME, Name, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	return SessionInterface->CreateSession(0, FName(Name), SessionSettings);
}

bool UNetworkSubsystem::FindLobbies()
{
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->bIsLanQuery = OnlineSubsystem->GetSubsystemName() == "Steam";
	SessionSearch->MaxSearchResults = 10000;

	// Switch based on subsystem
	if (OnlineSubsystem->GetSubsystemName() == "Steam")
	{
		SessionSearch->bIsLanQuery = false;
		SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
	}
	else
	{
		SessionSearch->bIsLanQuery = true;
		SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, false, EOnlineComparisonOp::Equals);
	}

	return SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
}

bool UNetworkSubsystem::JoinLobby(const FString Name, const FBlueprintSessionResult& Result)
{
	return SessionInterface->JoinSession(0, FName(Name), Result.OnlineResult);
}

bool UNetworkSubsystem::DestroyLobby(const FString Name)
{
	return SessionInterface->DestroySession(FName(Name));
}

void UNetworkSubsystem::OnError(const FString& Error)
{
	// Temporarily debug print
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, Error);
	OnLobbyErrorDelegate.Broadcast(Error);
}

void UNetworkSubsystem::OnCreateSessionComplete(FName SessionName, bool WasSuccessful)
{
	if (!WasSuccessful)
	{
		OnError("Failed to create session");
		OnLobbyCreatedDelegate.Broadcast(*SessionName.ToString(), false);
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("Created session %s"), *SessionName.ToString());
	FString MapName = "";
	SessionSettings.Get(SETTING_MAPNAME, MapName);
	GetWorld()->ServerTravel("/Game/Maps/" + MapName + "?listen");

	OnLobbyCreatedDelegate.Broadcast(*SessionName.ToString(), true);
}

void UNetworkSubsystem::OnFindSessionsComplete(bool WasSuccessful)
{
	TArray<FServerDetails> FoundLobbies;

	const auto SearchResults = SessionSearch->SearchResults;
	UE_LOG(LogTemp, Warning, TEXT("Found %d lobbies"), SearchResults.Num());
	if (SearchResults.Num() == 0)
	{
		OnError("No lobbies found");
		OnLobbyFoundDelegate.Broadcast(FoundLobbies, false);
		return;
	}

	// Convert session results to server details
	for (const auto& SearchResult : SearchResults)
	{
		FServerDetails Details;
		Details.Owner = SearchResult.Session.OwningUserName;

		SearchResult.Session.SessionSettings.Get(SETTING_SERVERNAME, Details.Name);
		SearchResult.Session.SessionSettings.Get(SETTING_MAPNAME, Details.Map);

		FBlueprintSessionResult BPSessionResult;
		BPSessionResult.OnlineResult = SearchResult;
		Details.Session = BPSessionResult;
		FoundLobbies.Add(Details);
	}

	OnLobbyFoundDelegate.Broadcast(FoundLobbies, WasSuccessful);
}

void UNetworkSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (Result != EOnJoinSessionCompleteResult::Success)
	{
		switch (Result)
		{
		case EOnJoinSessionCompleteResult::SessionIsFull:
			OnError("Session is full");
			break;
		case EOnJoinSessionCompleteResult::SessionDoesNotExist:
			OnError("Session no longer exists");
			break;
		case EOnJoinSessionCompleteResult::CouldNotRetrieveAddress:
			OnError("Could not retrieve address");
			break;
		default:
			OnError("Failed to join session");
			break;
		}

		OnLobbyJoinedDelegate.Broadcast(SessionName.ToString(), false);
		return;
	}

	// Travel to the session
	FString ConnectString;
	SessionInterface->GetResolvedConnectString(SessionName, ConnectString);
	if (ConnectString.IsEmpty())
	{
		OnError("Could not retrieve connect string");
		OnLobbyJoinedDelegate.Broadcast(SessionName.ToString(), false);
		return;
	}

	APlayerController* PlayerController = GEngine->GetFirstLocalPlayerController(GetWorld());
	PlayerController->ClientTravel(ConnectString, ETravelType::TRAVEL_Absolute);

	OnLobbyJoinedDelegate.Broadcast(SessionName.ToString(), true);
}