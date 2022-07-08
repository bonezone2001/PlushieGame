#include "RaceGM.h"
#include "PlushGame/Gamemodes/Base/MasterPC.h"
#include "PlushGame/Pawns/Plushie/PlushPawn.h"

void ARaceGM::EndCountdown()
{
	Super::EndCountdown();

	// Get player pawn
	for (auto& Player : Players)
	{
		APlushPawn* PlushPawn = Cast<APlushPawn>(Player->GetPawn());
		if (PlushPawn)
			PlushPawn->GetMesh()->SetSimulatePhysics(true);
	}
}
