// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PlushiePlayer.generated.h"

UCLASS()
class PLUSHIEGAME_API APlushiePlayer : public APawn
{
	GENERATED_BODY()

	// Player mesh
	UPROPERTY(EditAnywhere, Category = "Player")
	class UStaticMeshComponent* playerMesh;

	// Player boom arm
	UPROPERTY(VisibleDefaultsOnly, Category = "Player")
	class USpringArmComponent* springArm;

	// Player camera
	UPROPERTY(VisibleDefaultsOnly, Category = "Player")
	class UCameraComponent* camera;

	UPROPERTY(EditAnywhere, Category = "Camera")
	float minZoom;

	UPROPERTY(EditAnywhere, Category = "Camera")
	float maxZoom;

	UPROPERTY(EditAnywhere, Category = "Camera")
	float zoomSpeed;


public:
	// Sets default values for this pawn's properties
	APlushiePlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Turn pitch
	virtual void TurnPitch(float value);

	// Turn yaw
	virtual void TurnYaw(float value);

	// Zoom
	virtual void Zoom(float value);

	// Power
	virtual void TogglePower();
};
