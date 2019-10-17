// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Components/SphereComponent.h"
#include "AIController.h"
#include "MainCharacter.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/SkeletalMeshComponent.h"
#include "MainCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Sound/SoundCue.h"
#include "Animation/AnimInstance.h"
#include "TimerManager.h"
#include "Components/CapsuleComponent.h"
#include "Engine/World.h"
#include "Item.h"
#include "MainPlayerController.h"



// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	
	AgroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AgroSphere")); // Create a USphereComponent in the class (Will show up on the blueprint once a blueprint class is created)
	AgroSphere->SetupAttachment(GetRootComponent()); //Attached AgroSpehere to the RootComponent of the Enemy.  Character components should attach to the Root instead of trying to set as Root
	AgroSphere->InitSphereRadius(600.0f); //Set radius of AgroSphere at 600 from center of RootComponent
	
	CombatSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CombatSphere")); // Create a USphereComponent in the class (Will show up on the blueprint once a blueprint class is created)
	CombatSphere->SetupAttachment(GetRootComponent());  //Attached CombatSpehere to the RootComponent of the Enemy.  Character components should attach to the Root instead of trying to set as Root
	CombatSphere->InitSphereRadius(75.0f); //Set radius of AgroSphere at 600 from center of RootComponent

	// Create a box collision volume component
	CombatCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("CombatCollision"));

	// Attached the box collision volume to the a Socket called EnemySocket on the Enemy Mesh 
	CombatCollision->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("EnemySocket"));

	bOverlappingCombatSphere = false; 

	Health = 75.0f;
	MaxHealth = 100.0f; 
	Damage = 10.0f; 

	AttackMinTime = 0.5f;
	AttackMaxTime = 3.0f;

	EnemyMovementStatus = EEnemyMovementStatus::EMS_Idle;

	DeathDelay = 3.0f; 

	}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	// Assign the Controller of enemey to the AIController on BeginPlay, providing a reference to the AIController
	AIController = Cast<AAIController>(GetController());

	// Bind Overlap Events to Overlap Component functions
	AgroSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::AgroSphereOnOverlapBegin);
	AgroSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::AgroSphereOnOverlapEnd);

	CombatSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatSphereOnOverlapBegin);
	CombatSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::CombatSphereOnOverlapEnd);

	CombatCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatOnOverlapBegin);
	CombatCollision->OnComponentEndOverlap.AddDynamic(this, &AEnemy::CombatOnOverlapEnd);

	// Enable collision to NoCollision
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Set Collision Object Type to WorldDynamic
	CombatCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);

	// Ignore all collisions
	CombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	// Do NOT ignore Pawns and respond with an Overlap call
	CombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::AgroSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && Alive())
	{
		AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);

		if (MainCharacter)
		{
			MoveToTarget(MainCharacter);
		}
		
	}
	
}

void AEnemy::AgroSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);

		if (MainCharacter)
		{
			if (MainCharacter->CombatTarget == this)
			{
				MainCharacter->SetCombatTarget(nullptr);
			}
			MainCharacter->SetHasCombatTarget(false);

			// Check if there is a MainPlayerController assigned to MainCharacter
			if (MainCharacter->MainPlayerController)
			{
				// Call RemoveEnemyHealthBar from the MainPlayerControllerClass when moving out of the Agro Sphere
				MainCharacter->MainPlayerController->RemoveEnemyHealthBar(); 
			}

			SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Idle);
			
			if (AIController)
			{
				// Stop Enemy from chasing MainCharacter
				AIController->StopMovement();
			}
			
		}

	}
}

void AEnemy::CombatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
	UE_LOG(LogTemp, Warning, TEXT("In Enemy.cpp->CombatSphereOverlapBegin_01"));
	if (OtherActor && Alive())
	{
		UE_LOG(LogTemp, Warning, TEXT("In Enemy.cpp->CombatSphereOverlapBegin_02"));
		AMainCharacter* Main = Cast<AMainCharacter>(OtherActor);
		if (Main)
		{
			UE_LOG(LogTemp, Warning, TEXT("In Enemy.cpp->CombatSphereOverlapBegin_03"));
			Main->SetCombatTarget(this); 
			Main->SetHasCombatTarget(true);
			
			// Check if there is a MainPlayerController assigned to Main
			if (Main->MainPlayerController)
			{
				UE_LOG(LogTemp, Warning, TEXT("In Enemy.cpp->CombatSphereOverlapBegin_04"));
				
				// Display Enemy Health bar (calls function in MainPlayerController)
				Main->MainPlayerController->DisplayEnemyHealthBar(); 
			}

			CombatTarget = Main; 
			bOverlappingCombatSphere = true;
			Attack();
		}
	}
}

void AEnemy::CombatSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);

		if (MainCharacter)
		{
			bOverlappingCombatSphere = false; 
			if(EnemyMovementStatus != EEnemyMovementStatus::EMS_Attacking)
			{
				MoveToTarget(MainCharacter);
				CombatTarget = nullptr; 
			}

			// Clear attack timer set in AttackEnd() to prevent Enemey from attacking after MainCharacter moves away
			GetWorldTimerManager().ClearTimer(AttackTimer);
		
		}
		
	}
}

void AEnemy::MoveToTarget(AMainCharacter* Target)
{
	// SetEnemyMoveStatus to MoveToTarget
	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_MoveToTarget);

	if (AIController)
	{

		// Move to Target
		// Creating input parameters for AAIController::MoveTo function
		FAIMoveRequest MoveRequest;
		MoveRequest.SetGoalActor(Target);
		MoveRequest.SetAcceptanceRadius(10.0f);

		FNavPathSharedPtr NavPath; 

		// Call AIController::MoveTo function
		AIController->MoveTo(MoveRequest, &NavPath);

		// Copy TArray<FNavPathPoint> from NavPath that was created during call of MoveTo(MoveRequest, &NavPath)
		//TArray<FNavPathPoint> PathPoints = NavPath->GetPathPoints(); 

		// Iterate through each PathPoint in the TArray and display a DebugSphere at each one
		
		// Create a variable for the object in the TArray and assign the object in TArray to the variable.  Loop for each object in the TArray
		/**for (auto Point : PathPoints)
		{
			FVector Location = Point.Location;

			// Will draw a Green circle on the screen for 15 seconds 
			UKismetSystemLibrary::DrawDebugSphere(this, Location, 25.0f, 8, FLinearColor::Red, 15.0f, 1.5f);
		}

		*/


	}
}

void AEnemy::CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AMainCharacter* Main = Cast<AMainCharacter>(OtherActor);

		if (Main)
		{
			if (Main->HitParticles)
			{
				// Store the WeaponSocket socket from the weapon to WeaponSocket for reference
				const USkeletalMeshSocket* TipSocket = GetMesh()->GetSocketByName("TipSocket");

				if (TipSocket)
				{
					//Return the location of the WeaponSocket on the SkeletalMesh and store in SocketLocation
					FVector SocketLocation = TipSocket->GetSocketLocation(GetMesh());

					// Spawn an emitter at the location of WeaponSocket
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Main->HitParticles, SocketLocation, FRotator(0.0f), false);
				}


			}
			if (Main->HitSound)
			{
				UGameplayStatics::PlaySound2D(this, Main->HitSound);
			}
			if (DamageTypeClass)
			{
				UGameplayStatics::ApplyDamage(Main, Damage, AIController, this, DamageTypeClass);
			}

		}
	}

}

void AEnemy::CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void AEnemy::ActivateCollision()
{
	// Enable collision to QueryOnly
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	// Play the sound of the enemy attacking
	if (SwingSound)
	{
		UGameplayStatics::PlaySound2D(this, SwingSound);
	}

}

void AEnemy::DeactivateCollision()
{
	// Enable collision to NoCollision
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemy::Attack()
{
	
	//UE_LOG(LogTemp, Warning, TEXT("In Enemy::Attack()"));

	if (Alive())
	{
		// Check if there is an AIController
		if (AIController)
		{
			// Stop moving
			AIController->StopMovement();

			// Set EnemyMovementStatus to attacking
			SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Attacking);

		}

		// If not already attacking start combat montage
		if (!bAttacking)
		{
			bAttacking = true;

			// Play animation from combat montage
			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

			// Check if an AnimInstance is set
			if (AnimInstance)
			{
				AnimInstance->Montage_Play(CombatMontage, 1.35f);

				// Play the section called Attack from the SpiderCombatMontage
				AnimInstance->Montage_JumpToSection(FName("Attack"), CombatMontage);

			}

		}
	}
	
}

void AEnemy::AttackEnd()
{
	bAttacking = false; 

	// If MainCharacter is overlapping the combat sphere call attack
	if (bOverlappingCombatSphere)
	{
		// Create a random value between the minimum attack time and them maximum attack time
		float AttackTime = FMath::FRandRange(AttackMinTime, AttackMaxTime);

		// Start a timer and call AEnemy::Attack once the timer reaches AttackTime (0.5 to 3.0)
		GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, AttackTime);
	}
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
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

	return DamageAmount;
}

void AEnemy::Die()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		// Check if an AnimInstance is set
		if (AnimInstance)
		{
			AnimInstance->Montage_Play(CombatMontage, 1.35f);

			// Play the section called Death from the SpiderCombatMontage
			AnimInstance->Montage_JumpToSection(FName("Death"), CombatMontage);
		}

		// Set Enemy status to Dead
		SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Dead);

		// Disable all collision volumes 
		CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		AgroSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		CombatSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AEnemy::DeathEnd()
{
	UE_LOG(LogTemp, Warning, TEXT("In DeathEnd()"));
	
	// Stop animations after death
	GetMesh()->bPauseAnims = true;
	GetMesh()->bNoSkeletonUpdate = true; 

	// Set and begin timer, will call Disappear once timer runs out
	GetWorldTimerManager().SetTimer(DeathTimer, this, &AEnemy::Disappear, DeathDelay); 
}

bool AEnemy::Alive()
{
	return GetEnemyMovementStatus() != EEnemyMovementStatus::EMS_Dead;
}

void AEnemy::Disappear()
{
	UE_LOG(LogTemp, Warning, TEXT("In Disappear()"));

	Destroy(); 
}

