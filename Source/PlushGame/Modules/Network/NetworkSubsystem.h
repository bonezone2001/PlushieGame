// Made sense to convert my componment into a subsystem.
// That way it will always exist and be available.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "PlushGame/Structs/ServerDetails.h"
#include "NetworkSubsystem.generated.h"

#define SETTING_SERVERNAME FName(TEXT("SERVERNAME"))

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSessionsFound, const TArray<FServerDetails>&, Results, bool, WasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSessionCreate, FString, SessionName, bool, WasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSessionJoin, FString, SessionName, bool, WasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSessionError, FString, Error);

/**
 * 
 */
UCLASS()
class PLUSHGAME_API UNetworkSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
	FOnlineSessionSettings SessionSettings;
	TSharedPtr<FOnlineSessionSearch> SessionSearch;
	class IOnlineSubsystem* OnlineSubsystem;
	IOnlineSessionPtr SessionInterface;

public:
	UPROPERTY(BlueprintAssignable, Category=EventDispatchers)
	FOnSessionsFound OnLobbyFoundDelegate;

	UPROPERTY(BlueprintAssignable, Category=EventDispatchers)
	FOnSessionCreate OnLobbyCreatedDelegate;

	UPROPERTY(BlueprintAssignable, Category=EventDispatchers)
	FOnSessionJoin OnLobbyJoinedDelegate;

	UPROPERTY(BlueprintAssignable, Category=EventDispatchers)
	FOnSessionError OnLobbyErrorDelegate;

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	virtual void OnCreateSessionComplete(FName SessionName, bool WasSuccessful);
	virtual void OnFindSessionsComplete(bool WasSuccessful);
	virtual void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

public:
	void HandleNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString);
	void HandleTravelFailure(UWorld* World, ETravelFailure::Type FailureType, const FString& ErrorString);

	UFUNCTION(BlueprintCallable, Category=Network)
	bool LaunchLobby(const FString Name, const FString Map, const int32 MaxPlayers);

	UFUNCTION(BlueprintCallable, Category=Network)
	bool JoinLobby(const FString Name, const FBlueprintSessionResult& Result);

	UFUNCTION(BlueprintCallable, Category=Network)
	bool FindLobbies();

	UFUNCTION(BlueprintCallable, Category=Network)
	bool DestroyLobby(const FString Name);

	void OnError(const FString& Error);
};
