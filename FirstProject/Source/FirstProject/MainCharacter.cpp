// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacter.h"
#include "GameFramework/Controller.h"
#include "Gameframework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/World.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Weapon.h"
#include "Sound/SoundCue.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Enemy.h"
#include "MainPlayerController.h"
#include "FirstProjectSaveGame.h"
#include "ItemStorage.h"


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

	Attack2Damage = 10; 
	bAttack2 = false; 

	// Initialize ENUMS
	MovementStatus = EMovementStatus::EMS_Normal;
	StaminaStatus = EStaminaStatus::ESS_Normal;
	
	StaminaDrainRate = 25.0f; 
	MinSprintStamina = 50.0f; 

	InterpSpeed = 15.0f; 
	bInterpToEnemy = false; 

	bHasCombatTarget = false; 

	bMovingForward = false; 
	bMovingRight = false; 

}


// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();

	MainPlayerController = Cast<AMainPlayerController>(GetController());
	
}

// Called every frame
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MovementStatus == EMovementStatus::EMS_Dead)
	{
		return; 
	}
	
	float DeltaStamina = StaminaDrainRate * DeltaTime;
	
	//UE_LOG(LogTemp, Warning, TEXT("ActionDown = %s!"), bActionDown ? TEXT("True") : TEXT("False"));

	//UE_LOG(LogTemp, Warning, TEXT("Location = %s!"), *this->GetActorLocation().ToString());

	// Update the Stamina bar every tick
	switch (StaminaStatus)
	{
	case EStaminaStatus::ESS_Normal:
		if (bSprintingKeyPressed) 
		{
			
			UE_LOG(LogTemp, Warning, TEXT("In ESS_Normal->bSprintKeyPressed_01"));
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

			if (bMovingForward || bMovingRight)
			{
				UE_LOG(LogTemp, Warning, TEXT("In ESS_Normal->bSprintKeyPressed_02"));
				SetMovementStatus(EMovementStatus::EMS_Sprinting);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("In ESS_Normal->bSprintKeyPressed_03"));
				SetMovementStatus(EMovementStatus::EMS_Normal);
			}
			
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

				if (bMovingForward || bMovingRight)
				{
					SetMovementStatus(EMovementStatus::EMS_Sprinting);
				}
				else
				{
					SetMovementStatus(EMovementStatus::EMS_Normal);
				}
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

	if (bInterpToEnemy && CombatTarget)
	{
		// Identify and assign to a variable the Yaw value of the Enemy's location
		FRotator LookAtYaw = GetLookAtRotationYaw(CombatTarget->GetActorLocation());

		// Smoothly turn the MainCharacter to face the target
		// Identify and assign to a variable to amount of rotation for MainCharacter this frame
		FRotator InterpRotation = FMath::RInterpTo(GetActorRotation(), LookAtYaw, DeltaTime, InterpSpeed); 

		SetActorRotation(InterpRotation); 
	}

	// Check if there is an Enemy CombatTarget
	if (CombatTarget)
	{
		// Assign the FVector CombatTargetLocation variable to the location of the Enemy CombatTarget
		CombatTargetLocation = CombatTarget->GetActorLocation(); 
		
		if (MainPlayerController)
		{
			// Sets the variable EnemyLocation in teh MainPlayerController class the the location of the Enemy
			MainPlayerController->EnemyLocation = CombatTargetLocation;
		}
	}
}


// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Check that PlayerInput is valid
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMainCharacter::Jump);
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
	//PlayerInputComponent->BindAxis("MoveRight", this, &AMainCharacter::MoveRight);

	PlayerInputComponent->BindAxis("MoveRight", this, &AMainCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);

	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAxis("TurnRate", this, &AMainCharacter::TurnAtRate);

	PlayerInputComponent->BindAxis("LookUpRate", this, &AMainCharacter::LookUpAtRate);
}

void AMainCharacter::MoveForward(float value)
{
	bMovingForward = false; 
	if ((Controller != nullptr) && (value != 0.0f) && (!bAttacking) && (MovementStatus != EMovementStatus::EMS_Dead))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation(); 
		const FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f); 

		// Returns the X or forward direction from the current rotation or X axis
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		
		// Adding movement input in the direction calculated above
		AddMovementInput(Direction, value);

		//UE_LOG(LogTemp, Warning, TEXT("In MoveForward_01"));
		bMovingForward = true; 
	}
}

void AMainCharacter::MoveRight(float value)
{
	bMovingRight = false; 
	
	if ((Controller != nullptr) && (value != 0.0f), (!bAttacking) && (MovementStatus != EMovementStatus::EMS_Dead))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f);

		// Returns the Y or left/right direction from the current rotation or Y axis
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// Adding movement input in the direction calculated above
		AddMovementInput(Direction, value);

		UE_LOG(LogTemp, Warning, TEXT("In MoveRight_01"));
		bMovingRight = true; 
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

	if (MovementStatus == EMovementStatus::EMS_Dead)
	{
		return; 
	}

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
	bActionDown = false; 
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
	//UE_LOG(LogTemp, Warning, TEXT("In Attack()!, bAttacking is %s"), bAttacking ? TEXT("True") : TEXT("False"));

	if (!bAttacking && MovementStatus != EMovementStatus::EMS_Dead)
	{
		bAttacking = true;

		SetInterpToEnemy(true);
		
		// Define the AnimInstance with the AnimInstance associated with the Main Character
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && CombatMontage)
		{
			int32 Selection = FMath::RandRange(0, 1);  //create a random number between 0 and 1 (either 0 or 1)
			
			// switch statement, will run the appropriate case based on the value of Selection
			switch (Selection)
			{
			case 0:

				if (bAttack2)
				{
					bAttack2 = false; 
					EquippedWeapon->Damage -= Attack2Damage;
				}

				// Play Combat Montage
				AnimInstance->Montage_Play(CombatMontage, 2.2f); // Play the montage CombatMontage at 135% speed
				AnimInstance->Montage_JumpToSection(FName("Attack_1"), CombatMontage); // Play only the Attack_1 Section on CombatMontage
				break;
			case 1:
				
				if (!bAttack2)
				{
					bAttack2 = true;
					EquippedWeapon->Damage += Attack2Damage;
				}
				
				// Play Combat Montage
				AnimInstance->Montage_Play(CombatMontage, 1.5f); // Play the montage CombatMontage at 135% speed
				AnimInstance->Montage_JumpToSection(FName("Attack_2"), CombatMontage); // Play only the Attack_2 Section on CombatMontage
				break;
			
			default:
				;

			}
			
			
		}
				
	}
	


}

void AMainCharacter::AttackEnd()
{
	//UE_LOG(LogTemp, Warning, TEXT("In AttackEnd()!"));
	
	bAttacking = false; 
	bInterpToEnemy = false; 

	// if the ActionButton is held down it will automatically call Attack(); 
	if (bActionDown)
	{
		Attack(); 
	}
}

void AMainCharacter::PlaySwingSound()
{
	if (EquippedWeapon->SwingSound)
	{
		UGameplayStatics::PlaySound2D(this, EquippedWeapon->SwingSound);
	}
}


void AMainCharacter::IncrementCoin(int32 Amount)
{
	PowerCells += Amount; 

	UE_LOG(LogTemp, Warning, TEXT("Coins %d"), PowerCells);
	
}

void AMainCharacter::IncrementHealth(float Amount)
{
	if (Health + Amount >= MaxHealth)
	{
		Health = MaxHealth;
	}
	else
	{
		Health += Amount;
	}

}

void AMainCharacter::IncrementMana(float Amount)
{
	if (Mana + Amount >= MaxMana)
	{
		Mana = MaxMana;
	}
	else
	{
		Mana += Amount;
	}
}

// Reduce health when damage is taken
void AMainCharacter::DecrementHealth(float DamageAmount)
{
	if (Health - DamageAmount <= 0.0f)
	{
		Health -= DamageAmount;
		Die();
	}
	else
	{
		Health -= DamageAmount;
	}
}

float AMainCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (Health - DamageAmount <= 0.0f)
	{
		Health -= DamageAmount;
		Die();

		if (DamageCauser)
		{
			AEnemy* Enemy = Cast<AEnemy>(DamageCauser);
			if (Enemy)
			{
				Enemy->bHasValidTarget = false; 
			}
		}
	}
	else
	{
		Health -= DamageAmount;
	}

	return DamageAmount;
}

// Die when HP is <= 0
void AMainCharacter::Die()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && CombatMontage)
	{
		AnimInstance->Montage_Play(CombatMontage, 1.0f);
		AnimInstance->Montage_JumpToSection(FName("Death"));
	}
	SetMovementStatus(EMovementStatus::EMS_Dead);
}


void AMainCharacter::Jump()
{
	// Check if Dead, if so do nothing, if not call ACharacter::Jump(); 
	if (MovementStatus != EMovementStatus::EMS_Dead)
	{
		ACharacter::Jump(); 
	}
}

void AMainCharacter::DeathEnd()
{
	UE_LOG(LogTemp, Warning, TEXT("In DeathEnd()"));

	// Stop animations after death
	GetMesh()->bPauseAnims = true;
	GetMesh()->bNoSkeletonUpdate = true;
}

void AMainCharacter::UpdateCombatTarget()
{
	TArray<AActor*> OverlappingActors;

	// Get all actors that are of type Enemy that are overlapping with MainCharacter and store in TArray OverlapptingActors
	GetOverlappingActors(OverlappingActors, EnemyFilter);

	// Check to see if OverlappingActors TArray is empty
	if (OverlappingActors.Num() == 0)
	{
		if (MainPlayerController)
		{
			MainPlayerController->RemoveEnemyHealthBar(); 
		}
		return; 
	}
	
	AEnemy* ClosestEnemy = Cast<AEnemy>(OverlappingActors[0]);

	if (ClosestEnemy)
	{
		
		FVector Location = GetActorLocation(); 
		

		// Distance between the ClosestEnemy and the MainCharacter
		float MinDistance = (ClosestEnemy->GetActorLocation() - Location).Size();

		// Loop through OverlappingActors
		for (auto Actor : OverlappingActors)
		{
			AEnemy* Enemy = Cast<AEnemy>(Actor); 

			if (Enemy)
			{
				float DistanceToActor = (Enemy->GetActorLocation() - Location).Size();

				if (DistanceToActor < MinDistance)
				{
					MinDistance = DistanceToActor;
					ClosestEnemy = Enemy; 
				}
			}
			
			if (MainPlayerController)
			{
				MainPlayerController->DisplayEnemyHealthBar();
			}

			SetCombatTarget(ClosestEnemy);
			bHasCombatTarget = true; 

		}
	}
	
	

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

void AMainCharacter::SetInterpToEnemy(bool Interp)
{
	bInterpToEnemy = Interp;
}

FRotator AMainCharacter::GetLookAtRotationYaw(FVector Target)
{
	// Find rotation required to turn MainCharacter to Target
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target);

	// Obtain Yaw value of the LookAtRotation
	FRotator LookAtRotationYaw(0.0f, LookAtRotation.Yaw, 0.0f);

	return LookAtRotationYaw;
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

void AMainCharacter::SwitchLevel(FName LevelName)
{
	UWorld* World = GetWorld();

	if (World)
	{
		// Assign the name of the current level CurrentLevel
		FString CurrentLevel = World->GetMapName();

		FName CurrentLevelName(*CurrentLevel);

		if (CurrentLevelName != LevelName)
		{
			// Call funtion OpenLevel to open the new level
			UGameplayStatics::OpenLevel(World, LevelName);
		}
	}
}

void AMainCharacter::SaveGame()
{
	UFirstProjectSaveGame* SaveGameInstance = Cast<UFirstProjectSaveGame>(UGameplayStatics::CreateSaveGameObject(UFirstProjectSaveGame::StaticClass()));

	SaveGameInstance->CharacterStats.Health = Health; 
	SaveGameInstance->CharacterStats.MaxHealth = MaxHealth;
	SaveGameInstance->CharacterStats.Mana = Mana; 
	SaveGameInstance->CharacterStats.MaxMana = MaxMana;
	SaveGameInstance->CharacterStats.Stamina = Stamina;
	SaveGameInstance->CharacterStats.MaxStamina = MaxStamina; 
	SaveGameInstance->CharacterStats.PowerCells = PowerCells; 

	SaveGameInstance->CharacterStats.Location = GetActorLocation();
	SaveGameInstance->CharacterStats.Rotation = GetActorRotation();

	if (EquippedWeapon)
	{
		SaveGameInstance->CharacterStats.WeaponName = EquippedWeapon->WeaponName; 
	}

	UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveGameInstance->PlayerName, SaveGameInstance->UserIndex);
}

void AMainCharacter::LoadGame(bool SetPostion)
{
	UFirstProjectSaveGame* LoadGameInstance = Cast<UFirstProjectSaveGame>(UGameplayStatics::CreateSaveGameObject(UFirstProjectSaveGame::StaticClass()));

	LoadGameInstance = Cast<UFirstProjectSaveGame>(UGameplayStatics::LoadGameFromSlot(LoadGameInstance->PlayerName, LoadGameInstance->UserIndex));

	Health = LoadGameInstance->CharacterStats.Health;
	MaxHealth = LoadGameInstance->CharacterStats.MaxHealth;
	Mana = LoadGameInstance->CharacterStats.Mana;
	MaxMana = LoadGameInstance->CharacterStats.MaxMana;
	Stamina = LoadGameInstance->CharacterStats.Stamina;
	MaxStamina = LoadGameInstance->CharacterStats.MaxStamina;
	PowerCells = LoadGameInstance->CharacterStats.PowerCells;
	
	if (WeaponStorage)
	{
		AItemStorage* Weapons = GetWorld()->SpawnActor<AItemStorage>(WeaponStorage);

		if (Weapons)
		{
			FString WeaponName = LoadGameInstance->CharacterStats.WeaponName; 

			//Spawn the weapon
			AWeapon* WeaponToEquip = GetWorld()->SpawnActor<AWeapon>(Weapons->WeaponMap[WeaponName]);

			WeaponToEquip->Equip(this);
		}
	}
	
	 if (SetPostion)
	{
		SetActorLocation(LoadGameInstance->CharacterStats.Location);
		SetActorRotation(LoadGameInstance->CharacterStats.Rotation);
	}
}
