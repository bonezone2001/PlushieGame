#include "PlushPawnMovementComponent.h"

#include "PlushPawn.h"
#include "Net/UnrealNetwork.h"

UPlushPawnMovementComponent::UPlushPawnMovementComponent(const FObjectInitializer& ObjectInitializer)
{
	PawnState = EPlushState::Idle;
	ChargeMultiplier = 1.f;
	Charge = 0.f;
	Cooldown = 0.f;
}

void UPlushPawnMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (GetOwnerRole() == ROLE_Authority)
		if (Cooldown > 0) Cooldown -= DeltaTime;

	if (Cooldown <= 0)
	{
		if (PawnState == EPlushState::Moving && GetOwner()->GetVelocity().Size() < 0.2f)
		{
			SetState(EPlushState::Idle);
			OnStoppedMoving.Broadcast();
		}
	}
}

void UPlushPawnMovementComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UPlushPawnMovementComponent, Cooldown);
	DOREPLIFETIME_CONDITION(UPlushPawnMovementComponent, PawnState, COND_SkipOwner);
}

void UPlushPawnMovementComponent::SetCharge(float NewCharge)
{
	Charge = NewCharge;
	OnChargeChanged.Broadcast(NewCharge);
}

void UPlushPawnMovementComponent::SetState(EPlushState NewState)
{
	PawnState = NewState;
	OnStateUpdate.Broadcast(NewState);
}

void UPlushPawnMovementComponent::ChargeBegin()
{
	if (PawnState != EPlushState::Idle) return;
	if (Cooldown > 0) return;
	SetState(EPlushState::Charging);
	SetCharge(0.f);

	// Set owner mesh to use controller yaw
	APlushPawn* Character = Cast<APlushPawn>(GetOwner());
	if (Character)
		Character->bUseControllerRotationYaw = true;
	
	if (GetOwnerRole() != ROLE_Authority)
		ServerChargeBegin();
	else
		Cooldown = 0.5f;
	OnChargeStart.Broadcast();
}

void UPlushPawnMovementComponent::ServerChargeBegin_Implementation()
{
	ChargeBegin();
}

void UPlushPawnMovementComponent::HandleCharging(float Value)
{
	if (PawnState != EPlushState::Charging) return;

	// Charge changed
	const float NewCharge = FMath::Clamp(Charge + Value, 0.f, 100.f);
	if (NewCharge != Charge)
		SetCharge(NewCharge);
	
	// Get yaw
	const FRotator Rotation = GetOwner()->GetActorRotation();
	if (GetOwnerRole() != ROLE_Authority)
		ServerHandleCharging(Charge, Rotation.Yaw);
}

void UPlushPawnMovementComponent::ServerHandleCharging_Implementation(float Value, float Yaw)
{
	HandleCharging(Value);

	FRotator Rotation = GetOwner()->GetActorRotation();
	Rotation.Yaw = Yaw;
	GetOwner()->SetActorRotation(Rotation);
}

void UPlushPawnMovementComponent::ChargeEnd()
{
	if (PawnState != EPlushState::Charging) return;
	if (Cooldown > 0) return;
	Cooldown = 1.5f;
	SetState(EPlushState::Moving);
	
	APlushPawn* Character = Cast<APlushPawn>(GetOwner());
	if (Character)
		Character->bUseControllerRotationYaw = false;

	if (GetOwnerRole() != ROLE_Authority)
	 	ServerChargeEnd(Charge);
	AddChargeImpulse();
	
	OnChargeRelease.Broadcast(Charge);
	SetCharge(0.f);
}

void UPlushPawnMovementComponent::ServerChargeEnd_Implementation(float Value)
{
	SetCharge(FMath::Clamp(Value, 0.f, 100.f));
	ChargeEnd();
}

void UPlushPawnMovementComponent::AddChargeImpulse()
{
	// Add physics impulse from charge inside pawn movement
    if (Charge > 0.f)
    {
    	UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(GetOwner()->GetRootComponent());
    	if (PrimitiveComponent)
    	{
    		const FVector Impulse = (GetOwner()->GetActorForwardVector() * 100) * Charge * ChargeMultiplier;
    		PrimitiveComponent->AddImpulse(Impulse, NAME_None, true);
    	}
    }
}

float UPlushPawnMovementComponent::GetCharge() const
{
	return Charge;
}

bool UPlushPawnMovementComponent::GetCharging() const
{
	return PawnState == EPlushState::Charging;
}

EPlushState UPlushPawnMovementComponent::GetPawnState() const
{
	return PawnState;
}

float UPlushPawnMovementComponent::GetChargeMultiplier() const
{
	return ChargeMultiplier;
}

void UPlushPawnMovementComponent::OnRep_State()
{
	OnStateUpdate.Broadcast(PawnState);
}