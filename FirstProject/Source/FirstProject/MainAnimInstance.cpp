// Fill out your copyright notice in the Description page of Project Settings.


#include "MainAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"

void UMainAnimInstance::NativeInitializeAnimation()
{
	if (Pawn == nullptr)
	{
		Pawn = TryGetPawnOwner();
	}


}

void UMainAnimInstance::UpdateAnimationProperties()
{
	if (Pawn == nullptr)
	{
		Pawn = TryGetPawnOwner();
	}

	if (Pawn)
	{
		// Get characters current velocity
		FVector Speed = Pawn->GetVelocity(); 
		FVector LateralSpeed = FVector(Speed.X, Speed.Y, 0.0); 
		MovementSpeed = LateralSpeed.Size(); 

		bIsInAir = Pawn->GetMovementComponent()->IsFalling();
	}
}
