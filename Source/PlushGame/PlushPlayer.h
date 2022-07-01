// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/Pawn.h"
#include "PlushPlayer.generated.h"

UENUM(BlueprintType)
enum class EPlushState : uint8
{
	Idle 			UMETA(DisplayName = "Idle"),
	Charging		UMETA(DisplayName = "Charging"),
	Moving			UMETA(DisplayName = "Moving")
};

UCLASS()
class PLUSHGAME_API APlushPlayer : public APawn
{
	GENERATED_BODY()

public:
	APlushPlayer();

	// Game
	UPROPERTY(BlueprintReadOnly, Replicated, Category = Game)
	EPlushState PlushState;

	UPROPERTY(BlueprintReadOnly, Replicated, Category=Game)
	int ChargePower;

	// Components
	UPROPERTY(EditAnywhere, Category = Player)
	class USphereComponent* Root;

	UPROPERTY(EditAnywhere, Category=Player)
	UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, Category=Camera)
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, Category=Camera)
	class UCameraComponent* Camera;

	// Settings
	UPROPERTY(EditAnywhere, Category=Camera)
	float ZoomSpeed;
	
	UPROPERTY(EditAnywhere, Category=Camera)
	float ZoomMin;
	
	UPROPERTY(EditAnywhere, Category=Camera)
	float ZoomMax;


	UPROPERTY(EditAnywhere, Category=Game)
	int ChargeSpeed;

protected:
	virtual void BeginPlay() override;

	void HandleCharging(float DeltaTime);
	void HandleMoving(float DeltaTime);

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/*
	 * Player controls
	 */
	UFUNCTION(BlueprintCallable, Category=Player)
	virtual void TurnYaw(float Value);
	
	UFUNCTION(BlueprintCallable, Category=Player)
	virtual void TurnPitch(float Value);
	
	UFUNCTION(BlueprintCallable, Category=Player)
	virtual void Zoom(float Value);
	
	UFUNCTION(BlueprintCallable, Category=Player)
	virtual void StartCharging();
	
	UFUNCTION(BlueprintCallable, Category=Player)
	virtual void StopCharging();


	/*
	 * Player Networking
	 */
	UFUNCTION(Server, Unreliable)
	void ServerUpdateYaw(float Value);
	
	UFUNCTION(Server, Reliable)
	void ServerBeginCharge();
	
	UFUNCTION(Server, Reliable)
	void ServerEndCharge();
};
