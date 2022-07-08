#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "PlushPawnMovementComponent.generated.h"

UENUM(BlueprintType)
enum class EPlushState : uint8
{
	Idle 			UMETA(DisplayName = "Idle"),
	Charging		UMETA(DisplayName = "Charging"),
	Moving			UMETA(DisplayName = "Moving")
};

// Charging delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FChargeStartDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FChargeReleaseDelegate, float, Charge);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FChargeChangedDelegate, float, Charge);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlushStateUpdate, EPlushState, State);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStoppedMovingDelegate);

/**
 * 
 */
UCLASS()
class PLUSHGAME_API UPlushPawnMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()

	UPROPERTY(ReplicatedUsing=OnRep_State, EditDefaultsOnly, Category=Movement)
	EPlushState PawnState;

	UPROPERTY(EditDefaultsOnly, Category=Movement)
	float ChargeMultiplier;

	UPROPERTY(EditDefaultsOnly, Category=Movement)
	float Charge;

	UPROPERTY(Replicated)
	float Cooldown;

	UPROPERTY(BlueprintAssignable, Category=Movement)
	FChargeStartDelegate OnChargeStart;

	UPROPERTY(BlueprintAssignable, Category=Movement)
	FChargeReleaseDelegate OnChargeRelease;

	UPROPERTY(BlueprintAssignable, Category=Movement)
	FChargeChangedDelegate OnChargeChanged;

	UPROPERTY(BlueprintAssignable, Category=Movement)
	FStoppedMovingDelegate OnStoppedMoving;

	UPROPERTY(BlueprintAssignable, Category=Movement)
	FPlushStateUpdate OnStateUpdate;

protected:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void SetCharge(float NewCharge);
	void SetState(EPlushState NewState);
	
public:
	UPlushPawnMovementComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category=Movement)
	virtual void ChargeBegin();

	UFUNCTION(Server, Reliable)
	virtual void ServerChargeBegin();
	
	UFUNCTION(BlueprintCallable, Category=Movement)
	virtual void HandleCharging(float Value);
	
	UFUNCTION(Server, Unreliable, Category=Movement)
	virtual void ServerHandleCharging(float Value, float Yaw);

	UFUNCTION(BlueprintCallable, Category=Movement)
	virtual void ChargeEnd();
	
	UFUNCTION(Server, Reliable)
	virtual void ServerChargeEnd(float Value);
	
	UFUNCTION(Category=Movement)
	virtual void AddChargeImpulse();

public:
	UFUNCTION(BlueprintCallable, Category=Movement)
	virtual float GetCharge() const;

	UFUNCTION(BlueprintCallable, Category=Movement)
	virtual bool GetCharging() const;

	UFUNCTION(BlueprintCallable, Category=Movement)
	virtual EPlushState GetPawnState() const;

	UFUNCTION(BlueprintCallable, Category=Movement)
	virtual float GetChargeMultiplier() const;

	UFUNCTION()
	virtual void OnRep_State();
};
