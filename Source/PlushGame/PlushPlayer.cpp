// Fill out your copyright notice in the Description page of Project Settings.


#include "PlushPlayer.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Net/UnrealNetwork.h"

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
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->AlwaysLoadOnClient = true;
	Mesh->AlwaysLoadOnServer = true;
	Mesh->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
	RootComponent = Mesh;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->SetupAttachment(Mesh);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);

	// Enable physics
	Mesh->SetSimulatePhysics(true);
	Mesh->SetLinearDamping(0.6f);
	Mesh->SetAngularDamping(0.6f);

	// Zoom defaults
	ZoomMin = 150.0f;
	ZoomMax = 400.0f;
	ZoomSpeed = 75.0f;

	// Game defaults
	ChargeSpeed = 750;
}

// Called when the game starts or when spawned
void APlushPlayer::BeginPlay()
{
	Super::BeginPlay();
}

void APlushPlayer::HandleCharging(float DeltaTime)
{
	ChargePower += DeltaTime * ChargeSpeed;
}

void APlushPlayer::HandleMoving(float DeltaTime)
{
	if (GetVelocity().Size() <= 0.1f)
		PlushState = EPlushState::Idle;
}

void APlushPlayer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APlushPlayer, PlushState);
	DOREPLIFETIME(APlushPlayer, ChargePower);
}

// Called every frame
void APlushPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Handle charging speed
	switch (PlushState)
	{
	case EPlushState::Charging:
		HandleCharging(DeltaTime);
		break;
	case EPlushState::Moving:
		HandleMoving(DeltaTime);
		break;
	default:
		break;
	}
}

// Called to bind functionality to input
void APlushPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("TurnYaw", this, &APlushPlayer::TurnYaw);
	PlayerInputComponent->BindAxis("TurnPitch", this, &APlushPlayer::TurnPitch);
	PlayerInputComponent->BindAxis("Scroll", this, &APlushPlayer::Zoom);

	PlayerInputComponent->BindAction("LeftClick", IE_Pressed, this, &APlushPlayer::StartCharging);
	PlayerInputComponent->BindAction("LeftClick", IE_Released, this, &APlushPlayer::StopCharging);
}

void APlushPlayer::TurnYaw(float Value)
{
	if (Value == 0) return;

	// Rotate mesh with camera if charging
	if (PlushState == EPlushState::Charging)
	{
		AddControllerYawInput(-Value);
		//ServerUpdateYaw(GetActorRotation().Yaw);
	}
	else
		AddControllerYawInput(Value);
}

void APlushPlayer::TurnPitch(float Value)
{
	// TODO: Network camera for spectating
	AddControllerPitchInput(Value);
}

void APlushPlayer::Zoom(float Value)
{
	SpringArm->TargetArmLength = FMath::Clamp(SpringArm->TargetArmLength + Value * ZoomSpeed, ZoomMin, ZoomMax);
}

void APlushPlayer::StartCharging()
{
	if (PlushState != EPlushState::Idle)
		return;

	if (!HasAuthority())
		ServerBeginCharge();

	if (PlushState == EPlushState::Idle)
	{
		PlushState = EPlushState::Charging;
		bUseControllerRotationYaw = true;
	}
}

void APlushPlayer::StopCharging()
{
	if (PlushState != EPlushState::Charging)
		return;

	if (!HasAuthority())
		ServerEndCharge();

	bUseControllerRotationYaw = false;
	if (ChargePower > 0.0f)
	{
		const FVector Force = GetActorForwardVector() * ChargePower;
		Mesh->SetPhysicsLinearVelocity(Force);
		ChargePower = 0.0f;
		PlushState = EPlushState::Moving;
	}
	else
		PlushState = EPlushState::Idle;
}

void APlushPlayer::ServerUpdateYaw_Implementation(float Value)
{
	SetActorRotation(FRotator(0.0f, Value, 0.0f));
}

void APlushPlayer::ServerBeginCharge_Implementation()
{
	StartCharging();
}

void APlushPlayer::ServerEndCharge_Implementation()
{
	StopCharging();
}