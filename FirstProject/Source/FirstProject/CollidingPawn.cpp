// Fill out your copyright notice in the Description page of Project Settings.


#include "CollidingPawn.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "CollidingMovementComponent.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
ACollidingPawn::ACollidingPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->SetupAttachment(GetRootComponent());
	SphereComponent->InitSphereRadius(40.0f);
	SphereComponent->SetCollisionProfileName(TEXT("Pawn"));

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(GetRootComponent());

	CameraInput = FVector2D(0.0f, 0.0f);

	//Used to hard code a Mesh instead of using blueprint
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshComponentAsset(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere'"));

	if (MeshComponentAsset.Succeeded())
	{
		MeshComponent->SetStaticMesh(MeshComponentAsset.Object);
		MeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -40.0f));
		MeshComponent->SetWorldScale3D(FVector(0.8f, 0.8f, 0.8f));
	}

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->SetRelativeRotation(FRotator(-45.0f, 0.0f, 0.0f));
	SpringArm->TargetArmLength = 400.0f;
	SpringArm->bEnableCameraLag = true; 
	SpringArm->CameraLagSpeed = 3.0f; 

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);

	MovementComponent = CreateDefaultSubobject<UCollidingMovementComponent>(TEXT("MovementComponent")); 
	MovementComponent->UpdatedComponent = RootComponent; 
	AutoPossessPlayer = EAutoReceiveInput::Player0;

}

// Called when the game starts or when spawned
void ACollidingPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACollidingPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FRotator NewRotation = GetActorRotation();
	NewRotation.Yaw += CameraInput.X; 
	SetActorRotation(NewRotation);

	FRotator NewSpringArmRotation = SpringArm->GetComponentRotation(); 
	NewSpringArmRotation.Pitch = FMath::Clamp(NewSpringArmRotation.Pitch += CameraInput.Y, -80.0f, -15.0f); 
	SpringArm->SetWorldRotation(NewSpringArmRotation);

}

// Called to bind functionality to input
void ACollidingPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ACollidingPawn::MoveForward);

	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ACollidingPawn::MoveRight);

	PlayerInputComponent->BindAxis(TEXT("CameraPitch"), this, &ACollidingPawn::PitchCamera);

	PlayerInputComponent->BindAxis(TEXT("CameraYaw"), this, &ACollidingPawn::YawCamera);
}

UPawnMovementComponent* ACollidingPawn::GetMovementComponent() const
{
	return MovementComponent;
}

void ACollidingPawn::MoveForward(float input)
{
	FVector Forward = GetActorForwardVector(); 
	if (MovementComponent)
	{
		MovementComponent->AddInputVector(input *Forward);
	}
}

void ACollidingPawn::MoveRight(float input)
{
	FVector Right = GetActorRightVector();
	if (MovementComponent)
	{
		MovementComponent->AddInputVector(input * Right);
	}
	
}

void ACollidingPawn::PitchCamera(float AxisValue)
{
	CameraInput.Y = AxisValue;
}

void ACollidingPawn::YawCamera(float AxisValue)
{
	CameraInput.X = AxisValue; 
}

