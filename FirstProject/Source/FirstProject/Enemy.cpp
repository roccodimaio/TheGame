// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Components/SphereComponent.h"
#include "AIController.h"
#include "MainCharacter.h"
#include "Kismet/KismetSystemLibrary.h"


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

	bOverlappingCombatSphere = false; 
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
	if (OtherActor)
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
			SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Idle);
			
			// Stop Enemy from chasing MainCharacter
			AIController->StopMovement();
		}

	}
}

void AEnemy::CombatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AMainCharacter* Main = Cast<AMainCharacter>(OtherActor);
		if (Main)
		{
			CombatTarget = Main; 
			bOverlappingCombatSphere = true;
			SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Attacking); 
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
		
		}
		
	}
}

void AEnemy::MoveToTarget(AMainCharacter* Target)
{
	// SetEnemyMoveStatus to MoveToTarget
	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_MoveToTarget);

	UE_LOG(LogTemp, Warning, TEXT("In MoveToTarget()"));
	if (AIController)
	{
		//UE_LOG(LogTemp, Warning, TEXT("In MoveToTarget()_02"));

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

