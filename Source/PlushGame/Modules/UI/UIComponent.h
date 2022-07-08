#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UIComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PLUSHGAME_API UUIComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UUIComponent();
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Base class types
	UPROPERTY(EditAnywhere, Category = UI)
	TSubclassOf<UUserWidget> MainMenuWC;
	
	UPROPERTY(EditAnywhere, Category = UI)
	TSubclassOf<UUserWidget> HostGameMenuWC;
	
	UPROPERTY(EditAnywhere, Category = UI)
	TSubclassOf<UUserWidget> ServerListMenuWC;
	
	UPROPERTY(EditAnywhere, Category = UI)
	TSubclassOf<UUserWidget> OptionsMenuWC;

private:
	UUserWidget* CreateAndShowMenu(TSubclassOf<UUserWidget> MenuWidgetClass) const;

public:
	UFUNCTION(BlueprintCallable, Category=UI)
	void ShowMainMenu();
	
	UFUNCTION(BlueprintCallable, Category=UI)
	void ShowHostGameMenu();
	
	UFUNCTION(BlueprintCallable, Category=UI)
	void ShowOptionsMenu();
	
	UFUNCTION(BlueprintCallable, Category=UI)
	void ShowServerListMenu();
		
};
