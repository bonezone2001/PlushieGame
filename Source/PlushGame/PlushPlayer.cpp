// Fill out your copyright notice in the Description page of Project Settings.


#include "PlushPlayer.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"

// Sets default values
APlushPlayer::APlushPlayer()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	NetPriority = 3.0f;

	bCollideWhenPlacing = false;
	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// Setup components
	Root = CreateDefaultSubobject<USphereComponent>("Lol");
	Root->InitSphereRadius(35.0f);
	Root->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
	RootComponent = Root;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->AlwaysLoadOnClient = true;
	Mesh->AlwaysLoadOnServer = true;
	Mesh->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
	Mesh->SetupAttachment(RootComponent);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->SetupAttachment(RootComponent);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);

	// Zoom defaults
	ZoomMin = 150.0f;
	ZoomMax = 400.0f;
	ZoomSpeed = 75.0f;
}

// Called when the game starts or when spawned
void APlushPlayer::BeginPlay()
{
	Super::BeginPlay();
	
}
	
// Called every frame
void APlushPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APlushPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("TurnYaw", this, &APlushPlayer::TurnYaw);
	PlayerInputComponent->BindAxis("TurnPitch", this, &APlushPlayer::TurnPitch);
	PlayerInputComponent->BindAxis("Scroll", this, &APlushPlayer::Zoom);

	PlayerInputComponent->BindAction("LeftClick", IE_Pressed, this, &APlushPlayer::Charge);
}

void APlushPlayer::TurnYaw(float Value)
{
	AddControllerYawInput(Value);
}

void APlushPlayer::TurnPitch(float Value)
{
	AddControllerPitchInput(Value);
}

void APlushPlayer::Zoom(float Value)
{
	SpringArm->TargetArmLength = FMath::Clamp(SpringArm->TargetArmLength + Value * ZoomSpeed, ZoomMin, ZoomMax);
}

void APlushPlayer::Charge()
{
	UE_LOG(LogTemp, Warning, TEXT("Charging"));
}