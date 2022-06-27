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
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	UFUNCTION(BlueprintCallable, Category=Player)
	virtual void TurnYaw(float Value);
	
	UFUNCTION(BlueprintCallable, Category=Player)
	virtual void TurnPitch(float Value);
	
	UFUNCTION(BlueprintCallable, Category=Player)
	virtual void Zoom(float Value);
	
	UFUNCTION(BlueprintCallable, Category=Player)
	virtual void Charge();
};
