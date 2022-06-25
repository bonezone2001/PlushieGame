// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PlushPlayer.generated.h"

UCLASS()
class PLUSHGAME_API APlushPlayer : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APlushPlayer();

	UPROPERTY(EditAnywhere, Category = Player)
	class USphereComponent* Root;

	UPROPERTY(EditAnywhere, Category=Player)
	UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, Category=Camera)
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, Category=Camera)
	class UCameraComponent* Camera;
	
	UPROPERTY(EditAnywhere, Category=Camera)
	float ZoomSpeed;
	
	UPROPERTY(EditAnywhere, Category=Camera)
	float ZoomMin;
	
	UPROPERTY(EditAnywhere, Category=Camera)
	float ZoomMax;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	virtual void TurnYaw(float Value);

	virtual void TurnPitch(float Value);

	virtual void Zoom(float Value);

	virtual void Charge();
};
