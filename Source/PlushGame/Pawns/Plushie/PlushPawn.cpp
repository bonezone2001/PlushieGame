#include "PlushPawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "PlushPawnMovementComponent.h"
#include "Camera/CameraComponent.h"

APlushPawn::APlushPawn(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	// Set network settings
	bReplicates = true;
	bAlwaysRelevant = true;
	NetPriority = 5.0f;

	MovementComponent = CreateDefaultSubobject<UPlushPawnMovementComponent>(TEXT("MovementComponent"));
	MovementComponent->SetIsReplicated(true);

	// Root component is the mesh
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	//Mesh->SetSimulatePhysics(false);
	Mesh->BodyInstance.COMNudge = FVector(0.0f, 0.0f, -25.0f);
	Mesh->BodyInstance.SetMassOverride(0.5f, true);
	Mesh->SetLinearDamping(0.1f);
	Mesh->SetAngularDamping(0.1f);
	// Set center of mass inside of constructor
	
	RootComponent = Mesh;

	// Setup components
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);

	SpringArm->bUsePawnControlRotation = true;
	SpringArm->TargetArmLength = 300.f;
	SpringArm->SetRelativeLocation(FVector(0.f, 0.f, 50.f));

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);

	ZoomSpeed = 50.0f;
}

// Called when the game starts or when spawned
void APlushPawn::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void APlushPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void APlushPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Camera
	PlayerInputComponent->BindAxis("MouseX", this, &APlushPawn::TurnYaw);
	PlayerInputComponent->BindAxis("MouseY", this, &APlushPawn::TurnPitch);
	PlayerInputComponent->BindAxis("MouseWheel", this, &APlushPawn::DoZoom);

	// Character movement
	PlayerInputComponent->BindAxis("MouseY", this, &APlushPawn::ChargeChange);
	PlayerInputComponent->BindAction("LeftClick", IE_Pressed, this, &APlushPawn::ChargeStart);
	PlayerInputComponent->BindAction("LeftClick", IE_Released, this, &APlushPawn::ChargeStop);
}

void APlushPawn::TurnYaw(float Value)
{
	AddControllerYawInput(Value);
}

void APlushPawn::TurnPitch(float Value)
{
	if (MovementComponent->GetCharging()) return;
	AddControllerPitchInput(Value);
}

void APlushPawn::DoZoom(float Value)
{
	SpringArm->TargetArmLength = FMath::Clamp(SpringArm->TargetArmLength + -Value * ZoomSpeed, 150.0f, 500.0f);
}

void APlushPawn::ChargeChange(float Value)
{
	MovementComponent->HandleCharging(-Value);
}

void APlushPawn::ChargeStart()
{
	MovementComponent->ChargeBegin();
}

void APlushPawn::ChargeStop()
{
	MovementComponent->ChargeEnd();
}

UStaticMeshComponent* APlushPawn::GetMesh() const
{
	return Mesh;
}

UPlushPawnMovementComponent* APlushPawn::GetPlushMovementComponent() const
{
	return MovementComponent;
}
