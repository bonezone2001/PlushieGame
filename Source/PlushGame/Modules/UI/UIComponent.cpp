#include "UIComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PlushGame/Modules/Settings/PlushieSettings.h"

UUIComponent::UUIComponent()
{
	// Get developer settings
	const UPlushieSettings* PlushieSettings = GetDefault<UPlushieSettings>();
	MainMenuWC = PlushieSettings->MainMenuView;
	HostGameMenuWC = PlushieSettings->HostGameMenuView;
	ServerListMenuWC = PlushieSettings->ServerListMenuView;
	OptionsMenuWC = PlushieSettings->OptionsMenuView;
}

void UUIComponent::BeginPlay()
{
	Super::BeginPlay();
}

UUserWidget* UUIComponent::CreateAndShowMenu(TSubclassOf<UUserWidget> MenuWidgetClass) const
{
	if (!IsValid(MenuWidgetClass))
	{
		UE_LOG(LogTemp, Error, TEXT("MenuWidgetClass is not valid"));
		return nullptr;
	}

	UWorld* World = GetOuter()->GetWorld();
	if (!IsValid(World))
	{
		UE_LOG(LogTemp, Error, TEXT("World is not valid"));
		return nullptr;
	}

	UUserWidget* Widget = CreateWidget<UUserWidget>(World, MenuWidgetClass);
	Widget->AddToViewport();
	return nullptr;
}

void UUIComponent::ShowMainMenu()
{
	(void)CreateAndShowMenu(MainMenuWC);
}

void UUIComponent::ShowHostGameMenu()
{
	(void)CreateAndShowMenu(HostGameMenuWC);
}

void UUIComponent::ShowOptionsMenu()
{
	(void)CreateAndShowMenu(OptionsMenuWC);
}

void UUIComponent::ShowServerListMenu()
{
	(void)CreateAndShowMenu(ServerListMenuWC);
}