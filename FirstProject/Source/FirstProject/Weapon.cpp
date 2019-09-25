// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "MainCharacter.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"


AWeapon::AWeapon()
{
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMesh->SetupAttachment(GetRootComponent());
	
	bWaponParticles = false; 

	WeaponState = EWeaponState::EWS_Pickup; 

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
