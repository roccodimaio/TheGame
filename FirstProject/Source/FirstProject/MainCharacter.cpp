// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacter.h"
#include "GameFramework/Controller.h"
#include "Gameframework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/World.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"

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
	Stamina = 120.0f; 
	MaxStamina = 150.0f; 
	PowerCells = 1; 

	RunningSpeed = 650.0f; 
	SprintingSpeed = 900.0f; 

	bSprintingKeyPressed = false; 

	bActionDown = false; 

	// Initialize ENUMS
	MovementStatus = EMovementStatus::EMS_Normal;
	StaminaStatus = EStaminaStatus::ESS_Normal;
	
	StaminaDrainRate = 25.0f; 
	MinSprintStamina = 50.0f; 

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

	float DeltaStamina = StaminaDrainRate * DeltaTime;
	
	
	// Update the Stamina bar every tick
	switch (StaminaStatus)
	{
	case EStaminaStatus::ESS_Normal:
		if (bSprintingKeyPressed) // Sprinting key pressed
		{
			// Check if next change will put us into the BelowMinimum state
			if (Stamina - DeltaStamina <= MinSprintStamina)
			{
				SetStaminaStatus(EStaminaStatus::ESS_BelowMinimum);
				Stamina -= DeltaStamina; 
			}
			else
			{
				Stamina -= DeltaStamina;
			}

			SetMovementStatus(EMovementStatus::EMS_Sprinting);
		}
		else // Sprinting key not pressed
		{
			if (Stamina + DeltaStamina >= MaxStamina)
			{
				Stamina = MaxStamina; 
			}
			else
			{
				Stamina += DeltaStamina;
			}
			SetMovementStatus(EMovementStatus::EMS_Normal);
		}
		break;
	case EStaminaStatus::ESS_BelowMinimum:
		if (bSprintingKeyPressed) // Sprinting key pressed
		{
			if (Stamina - DeltaStamina <= 0.0f)
			{
				SetStaminaStatus(EStaminaStatus::ESS_Exhausted);
				Stamina = 0;
				SetMovementStatus(EMovementStatus::EMS_Normal);
			}
			else
			{
				Stamina -= DeltaStamina; 
				SetMovementStatus(EMovementStatus::EMS_Sprinting);
			}
		}
		else // Sprinting key not pressed
		{
			if (Stamina + DeltaStamina >= MinSprintStamina)
			{
				SetStaminaStatus(EStaminaStatus::ESS_Normal);
				Stamina += DeltaStamina;
			}
			else
			{
				Stamina += DeltaStamina; 
			}
			SetMovementStatus(EMovementStatus::EMS_Normal);
		}
		break;
	case EStaminaStatus::ESS_Exhausted:
		if (bSprintingKeyPressed)
		{
			Stamina = 0.0f;
		}
		else
		{
			SetStaminaStatus(EStaminaStatus::ESS_ExhaustedRecovering);
				Stamina += DeltaStamina; 
		}
		SetMovementStatus(EMovementStatus::EMS_Normal);
		break;
	case EStaminaStatus::ESS_ExhaustedRecovering:
		if (Stamina + DeltaStamina >= MinSprintStamina)
		{
			SetStaminaStatus(EStaminaStatus::ESS_Normal);
			Stamina += DeltaStamina; 
		}
		else
		{
			Stamina += DeltaStamina; 
		}
		SetMovementStatus(EMovementStatus::EMS_Normal);
		break;
	default:
		;
	}


}

// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Check that PlayerInput is valid
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AMainCharacter::SprintingKeyPressed);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AMainCharacter::SprintingKeyReleased);

	PlayerInputComponent->BindAction("Action", IE_Pressed, this, &AMainCharacter::ActionButtonPressed);
	PlayerInputComponent->BindAction("Action", IE_Released, this, &AMainCharacter::ActionButtonReleased);


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

void AMainCharacter::ActionButtonPressed()
{
	// Functionality to equip weapong
	bActionDown = true;
	if (ActiveOverlappingItem)  // if main character is overlapping an item
	{
		AWeapon* Weapon = Cast<AWeapon>(ActiveOverlappingItem);  // Create an instance of AWeapon and cast the overlapping item to it

		if (Weapon) // if the casting was successful
		{
			Weapon->Equip(this); // Call the Equip function within AWeapon and pass in the MainCharacter as the AActor parameter

			SetActiveOverlappingItem(nullptr); // Item is equipment and no longer overlapping.  Reset ActiveOverlappingItem to null
		}
	}
	else if(EquippedWeapon)  //if not overlapping an item and a weapon is equipped and ActionButton is pressed then execute code below
	{
		Attack();

	}
}

void AMainCharacter::ActionButtonReleased()
{

}

void AMainCharacter::EquippedWeapton(AWeapon* WeaponToSet)
{
	if (EquippedWeapon)
	{
		EquippedWeapon->Destroy();
	}

	EquippedWeapon = WeaponToSet; 
}

void AMainCharacter::Attack()
{
	bAttacking = true;

	// Define the AnimInstance with the AnimInstance associated with the Main Character
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && CombatMontage)
	{
		// Play Combat Montage
		AnimInstance->Montage_Play(CombatMontage, 1.35f); // Play the montage CombatMontage at 135% speed
		AnimInstance->Montage_JumpToSection(FName("Attack_1"), CombatMontage); // Play only the Attack_1 Section on CombatMontage
	}


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

void AMainCharacter::ShowPickupLocations()
{
	
	// Loop through TArray PickupLocations and draw a red debug circle at each location
	for (auto Location : PickupLocations)
	{
		// Will draw a red circle on the screen for 15 seconds 
		UKismetSystemLibrary::DrawDebugSphere(this, Location, 25.0f, 8, FLinearColor::Green, 15.0f, 0.5f);
	}
	
	
}

// Setting the movement status and changing the speed based on our state
void AMainCharacter::SetMovementStatus(EMovementStatus Status)
{
	MovementStatus = Status; 
	if (MovementStatus == EMovementStatus::EMS_Sprinting)
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintingSpeed; 
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = RunningSpeed; 
	}
}

void AMainCharacter::SprintingKeyPressed()
{
	bSprintingKeyPressed = true; 
}

void AMainCharacter::SprintingKeyReleased()
{
	bSprintingKeyPressed = false; 
}
