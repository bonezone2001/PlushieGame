#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "PlushieSettings.generated.h"

/**
 * Global settings for the Plushie Game.
 */
UCLASS(Config=Game, defaultconfig, meta=(DisplayName="Plushie Developer Settings"))
class PLUSHGAME_API UPlushieSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	// UI Settings
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category=UI)
	TSubclassOf<UUserWidget> MainMenuView;
	
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category=UI)
	TSubclassOf<UUserWidget> HostGameMenuView;
	
	UPROPERTY(Config, EditAnywhere,BlueprintReadOnly, Category=UI)
	TSubclassOf<UUserWidget> ServerListMenuView;
	
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category=UI)
	TSubclassOf<UUserWidget> OptionsMenuView;
};
