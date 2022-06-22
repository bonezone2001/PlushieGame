// Fill out your copyright notice in the Description page of Project Settings.


#include "PlushiePlayer.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

// Sets default values
APlushiePlayer::APlushiePlayer()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create root component
	//RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("PlushieRoot"));

	// Create mesh
	playerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	//playerMesh->SetupAttachment(RootComponent);

	// Default zoom
	minZoom = 150.0f;
	maxZoom = 400.0f;
	zoomSpeed = 25.f;

	// Create boom arm
	springArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	springArm->SetupAttachment(RootComponent);
	springArm->bUsePawnControlRotation = true;

	// Create camera
	camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	camera->SetupAttachment(springArm, USpringArmComponent::SocketName);
}

// Called when the game starts or when spawned
void APlushiePlayer::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APlushiePlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}

// Called to bind functionality to input
void APlushiePlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Bind camera events
	PlayerInputComponent->BindAxis("TurnYaw", this, &APlushiePlayer::TurnYaw);
	PlayerInputComponent->BindAxis("TurnPitch", this, &APlushiePlayer::TurnPitch);
	PlayerInputComponent->BindAxis("Zoom", this, &APlushiePlayer::Zoom);

	// Bind power
	PlayerInputComponent->BindAction("PlushiePower", IE_Pressed, this, &APlushiePlayer::TogglePower);
}

void APlushiePlayer::TurnPitch(float value)
{
	AddControllerPitchInput(value);
}

void APlushiePlayer::TurnYaw(float value)
{
	AddControllerYawInput(value);
}

void APlushiePlayer::Zoom(float value)
{
	springArm->TargetArmLength = FMath::Clamp(springArm->TargetArmLength + value * zoomSpeed, minZoom, maxZoom);
}

void APlushiePlayer::TogglePower()
{

}

