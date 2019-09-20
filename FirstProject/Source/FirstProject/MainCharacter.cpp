// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacter.h"
#include "GameFramework/Controller.h"
#include "Gameframework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/World.h"
#include "Camera/CameraComponent.h"

// Sets default values
AMainCharacter::AMainCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create Spring Arm. Pulls towards the player if there is a collision
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 600.0f; // Length of Spring Arm, camera will follow at this distance
	CameraBoom->bUsePawnControlRotation = true; // Rotate Spring Arm based on controller

	// Set size for collision capsule
	GetCapsuleComponent()->SetCapsuleSize(48.0f, 100.0f);
	
	// Create Camera to follow Main Character
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach Camera to the default socket at the end of the Spring Arm
	FollowCamera->bUsePawnControlRotation = false; // Let Spring Arm adjust to match the controller orientation 

	// Set turn rates for input
	BaseTurnRate = 65.0f; 
	BaseLookUpRate = 65.0f; 

	// Don't rotate MainCharacter when controller rotates
	// Should only affect the camera
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false; 
	bUseControllerRotationRoll = false; 

	// Configure character move to face direction of movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 250.0f; // Velocity at jump, will determine height
	GetCharacterMovement()->AirControl = 0.2f; // Allows control of character while in air.  Smaller number means less control

	MaxHealth = 100.0f;
	Health = 65.0f;
	MaxMana = 200.0f;
	Mana = 150.0f;
	Stamina = 200.0f; 
	MaxStamina = 300.0f; 
	PowerCells = 1; 

}


// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Check that PlayerInput is valid
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);


	/** Call MoveForward when any of the keys/buttons associated with the Axis Mapping "MoveForward" are pressed
	"MoveForward" is the name of Axis Mapping in Project Settings - Input.  
	*/
	PlayerInputComponent->BindAxis("MoveForward", this, &AMainCharacter::MoveForward);

	/** Call MoveRight when any of the keys/buttons associated with the Axis Mapping "MoveRight" are pressed
	"MoveRight" is the name of Axis Mapping in Project Settings - Input.
	*/
	PlayerInputComponent->BindAxis("MoveRight", this, &AMainCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);

	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAxis("TurnRate", this, &AMainCharacter::TurnAtRate);

	PlayerInputComponent->BindAxis("LookUpRate", this, &AMainCharacter::LookUpAtRate);
}

void AMainCharacter::MoveForward(float value)
{
	if ((Controller != nullptr) && (value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation(); 
		const FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f); 

		// Returns the X or forward direction from the current rotation or X axis
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		
		// Adding movement input in the direction calculated above
		AddMovementInput(Direction, value);
	}
}

void AMainCharacter::MoveRight(float value)
{
	if ((Controller != nullptr) && (value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f);

		// Returns the Y or left/right direction from the current rotation or Y axis
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// Adding movement input in the direction calculated above
		AddMovementInput(Direction, value);
	}
}

void AMainCharacter::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMainCharacter::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}


void AMainCharacter::IncrementCoin(int32 Amount)
{
	PowerCells += Amount; 

	UE_LOG(LogTemp, Warning, TEXT("Coins %d"), PowerCells);
	
}

// Reduce health when damage is taken
void AMainCharacter::DecrementHealth(float Amount)
{
	if (Health - Amount <= 0.0f)
	{
		Health -= Amount; 
		Die();
	}
	else
	{
		Health -= Amount; 
	}
}

// Die when HP is <= 0
void AMainCharacter::Die()
{

}
