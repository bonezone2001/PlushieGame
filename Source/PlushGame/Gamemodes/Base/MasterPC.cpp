#include "MasterPC.h"

#include "Kismet/GameplayStatics.h"
#include "PlushGame/Modules/UI/UIComponent.h"
#include "PlushGame/Pawns/Plushie/PlushPawn.h"

AMasterPC::AMasterPC(const FObjectInitializer& ObjectInitializer)
{
	UIComponent = CreateDefaultSubobject<UUIComponent>(TEXT("UIComponent"));
}

UUIComponent* AMasterPC::GetUI() const
{
	return UIComponent;
}

void AMasterPC::CountdownStarted_Implementation()
{
	OnCountdownStarted.Broadcast();
}

void AMasterPC::CountdownEnded_Implementation()
{
	// Get local player pawn
	APlushPawn* PlayerPawn = Cast<APlushPawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (IsValid(PlayerPawn))
		PlayerPawn->GetMesh()->SetSimulatePhysics(true);
	
	OnCountdownEnded.Broadcast();
}
