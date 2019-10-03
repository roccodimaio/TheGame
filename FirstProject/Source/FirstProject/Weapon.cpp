// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "MainCharacter.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/BoxComponent.h"
#include "Enemy.h"
#include "Engine/SkeletalMeshSocket.h"


AWeapon::AWeapon()
{
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMesh->SetupAttachment(GetRootComponent());

	CombatCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("CombatCollision"));
	CombatCollision->SetupAttachment(GetRootComponent()); 
	
	bWaponParticles = false; 

	WeaponState = EWeaponState::EWS_Pickup; 

	Damage = 25.0f; 

}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	// Call when CombatCollision box overlaps with a collision volume
	CombatCollision->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::CombatOnOverlapBegin);

	// Call when CombatCollision box overlaping with a collision volume ends
	CombatCollision->OnComponentEndOverlap.AddDynamic(this, &AWeapon::CombatOnOverlapEnd);

	// Enable collision to NoCollision
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Set Collision Object Type to WorldDynamic
	CombatCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);

	// Ignore all collisions
	CombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	// Do NOT ignore Pawns and respond with an Overlap call
	CombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	
}

void AWeapon::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	if ((WeaponState == EWeaponState::EWS_Pickup) && OtherActor)
	{
		AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);

		if (MainCharacter)
		{
			// Set the overlapping item to the OverlappingItem variable in MainCharacter
			MainCharacter->SetActiveOverlappingItem(this);
		}
	}
}

void AWeapon::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);

	if (OtherActor)
	{
		AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);

		if (MainCharacter)
		{
			MainCharacter->SetActiveOverlappingItem(nullptr);
		}
	}
}


void AWeapon::Equip(AMainCharacter* Char)
{
	if (Char)
	{
		// Set Camera to ignore the SkeletalMesh of the Weapon when coming between MainCharacter and the Camera
		SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

		// Ignore Collision with MainCharacter
		SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

		// Make sure physics is turned off
		SkeletalMesh->SetSimulatePhysics(false); 

		//Create a SkeletaMeshSocket and assign as the "RightHandSocket" on the MainCharacter Skeleton
		const USkeletalMeshSocket* RightHandSocket = Char->GetMesh()->GetSocketByName("RightHandSocket");

		// Attached SkeltalMesh to MainCharacter
		if (RightHandSocket)
		{
			RightHandSocket->AttachActor(this, Char->GetMesh());
			bRotate = false;

			// Destoy currently equiped weapon
			//Char->GetEquippedWeapon()->Destroy(); 

			// Set the EquipWeapon variable from MainCharacter to the attached Weapon
			Char->EquippedWeapton(this);
			Char->SetActiveOverlappingItem(nullptr);

		}

		if (OnEquipSound)

		{
			UGameplayStatics::PlaySound2D(this, OnEquipSound);
		}

		if (!bWaponParticles)
		{
			IdleParticlesComponent->Deactivate(); 
		}
	}
}

void AWeapon::CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
	if (OtherActor)
	{
		AEnemy* Enemy = Cast<AEnemy>(OtherActor);

		if (Enemy)
		{
			if (Enemy->HitParticles)
			{
				// Store the WeaponSocket socket from the weapon to WeaponSocket for reference
				const USkeletalMeshSocket* WeaponSocket = SkeletalMesh->GetSocketByName("WeaponSocket");

				if (WeaponSocket)
				{
					//Return the location of the WeaponSocket on the SkeletalMesh and store in SocketLocation
					FVector SocketLocation = WeaponSocket->GetSocketLocation(SkeletalMesh);

					// Spawn an emitter at the location of WeaponSocket
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Enemy->HitParticles, SocketLocation, FRotator(0.0f), false);
				}


			}
			if (Enemy->HitSound)
			{
				UGameplayStatics::PlaySound2D(this, Enemy->HitSound);
			}

		}
	}
	
}

void AWeapon::CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void AWeapon::ActivateCollision()
{
	// Enable collision to QueryOnly
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	
}

void AWeapon::DeactivateCollision()
{
	// Enable collision to NoCollision
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
