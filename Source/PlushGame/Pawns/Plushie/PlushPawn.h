#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PlushPawn.generated.h"



UCLASS()
class PLUSHGAME_API APlushPawn : public APawn
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category=Player)
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditDefaultsOnly, Category=Player)
	class USpringArmComponent* SpringArm;

	UPROPERTY(EditDefaultsOnly, Category=Player)
	class UCameraComponent* Camera;

	UPROPERTY(EditDefaultsOnly, Category=Player)
	class UPlushPawnMovementComponent* MovementComponent;

	UPROPERTY(EditDefaultsOnly, Category=Player)
	float ZoomSpeed;

public:
	APlushPawn(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	virtual void TurnYaw(float Value);

	UFUNCTION(BlueprintCallable)
	virtual void TurnPitch(float Value);
	
	UFUNCTION(BlueprintCallable)
	virtual void DoZoom(float Value);

	UFUNCTION(BlueprintCallable)
	virtual void ChargeChange(float Value);

	UFUNCTION(BlueprintCallable)
	virtual void ChargeStart();
	
	UFUNCTION(BlueprintCallable)
	virtual void ChargeStop();
	
public:	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	UFUNCTION(BlueprintCallable, Category=Getters)
	UStaticMeshComponent* GetMesh() const;

	UFUNCTION(BlueprintCallable, Category=Getters)
	class UPlushPawnMovementComponent* GetPlushMovementComponent() const;
};
