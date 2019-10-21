// Fill out your copyright notice in the Description page of Project Settings.


#include "Explosive.h"
#include "MainCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Sound/SoundCue.h"
#include "Enemy.h"

AExplosive::AExplosive()
{
	Damage = 15.0f; 
}

void AExplosive::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	//UE_LOG(LogTemp, Warning, TEXT("Explosive::OnOverLapBegin()"));

	// Check the actor that is overlapping
	if (OtherActor)
	{
		// Cast the retruned OtherActor to the MainCharacter which is an AMainCharater class
		AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);
		
		AEnemy* Enemy = Cast<AEnemy>(OtherActor);

		// Check if MainCharactor has been assigned a Character class
		if (MainCharacter || Enemy)
		{
			// Check if OverlapParticles has been set via blueprint
			if (OverlapParticles)
			{
				// Spawn particle OverlapParticles on OverlapBegin
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), OverlapParticles, GetActorLocation(), FRotator(0.0f), true);
			}

			if (OverlapSound)
			{
				UGameplayStatics::PlaySound2D(this, OverlapSound);
			}

			UGameplayStatics::ApplyDamage(OtherActor, Damage, nullptr, this, DamageTypeClass);
			
			Destroy();
		}
	}
}

void AExplosive::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
	
	UE_LOG(LogTemp, Warning, TEXT("Explosive::OnOverLapEnd()"));
}
