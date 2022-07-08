#include "MasterGS.h"
#include "Net/UnrealNetwork.h"

AMasterGS::AMasterGS()
{
	Countdown = 100;
	Round = 0;
}

void AMasterGS::OnRep_Countdown()
{
	UE_LOG(LogTemp, Warning, TEXT("Countdown: %d"), Countdown);
}

void AMasterGS::OnRep_Round()
{
	UE_LOG(LogTemp, Warning, TEXT("Round: %d"), Round);
}

void AMasterGS::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMasterGS, Countdown);
	DOREPLIFETIME(AMasterGS, Round);
}