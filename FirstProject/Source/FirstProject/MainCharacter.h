// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MainCharacter.generated.h"

/** create an enum with a class. Requires all calls to use fully qualified name to use. */
UENUM(BlueprintType)
enum class EMovementStatus : uint8
{
	EMS_Normal UMETA(DisplayName = "Normal"),
	EMS_Sprinting UMETA(DisplayName = "Sprinting"),
	EMS_MAX UMETA(DisplayName = "DefaultMAX"),
};

UENUM(BlueprintType)
enum class EStaminaStatus : uint8
{
	ESS_Normal UMETA(DisplayName = "Normal"),
	ESS_BelowMinimum UMETA(DisplayName = "BelowMinimum"),
	ESS_Exhausted UMETA(DisplayName = "Exhausted"),
	ESS_ExhaustedRecovering UMETA (DisplayName = "ExhaustedRecovering"),
	ESS_MAX UMETA(DisplayName = "DefaultMAX"),
};




UCLASS()
class FIRSTPROJECT_API AMainCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMainCharacter();

	// TArray container of type FVector
	TArray<FVector> PickupLocations; 
	
	UFUNCTION(BlueprintCallable)
	void ShowPickupLocations();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "ENUMs")
	EMovementStatus MovementStatus; 

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "ENUMs")
	EStaminaStatus StaminaStatus; 

	FORCEINLINE void SetStaminaStatus(EStaminaStatus Status)
	{
		StaminaStatus = Status; 
	}

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
	float StaminaDrainRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
	float MinSprintStamina; 


	// Set Movement status and running speed
	void SetMovementStatus(EMovementStatus Status);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sprinting")
	float RunningSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sprinting")
	float SprintingSpeed;

	bool bSprintingKeyPressed; 
	
	/**  Pressed down to enable sprinting */
	void SprintingKeyPressed(); 

	/** Released to strop sprinting */
	void SprintingKeyReleased(); 

	
	// Base turn rates to scale turning functions for the camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	float BaseTurnRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	float BaseLookUpRate;
	
	//Subobjects - Components within a blueprint

	// Add SpringArmComponent to MainCharacter - Used to position the camera behind the MainCharacter
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAcces = "true"))
	class USpringArmComponent* CameraBoom;

	// Add CameraComponent to MainCharacter - Camera to follow MainCharacter
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	

	/**


	PlayerStats

	*/

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerStats")
	float Health;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerStats")
	float MaxHealth; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerStats")
	float Mana;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerStats")
	float MaxMana;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerStats")
	float Stamina;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerStats")
	float MaxStamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerStats")
	int32 PowerCells; 

	void IncrementCoin(int32 Amount);

	void DecrementHealth(float Amount);

	void Die(); 


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Called for forwards/backwards input from keyboard, mouse, and controller
	void MoveForward(float value);

	// Called for side to side input from keyboard, mouse, and controller
	void MoveRight(float value);

	/** Called via input to turn a given rate
	*@param Rate This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void TurnAtRate(float Rate);

	/** Called via input to look up/down at a given rate
	*@param Rate This is a normalized rate, i.e. 1.0 means 100% of desired look up/down rate
	*/
	void LookUpAtRate(float Rate);

	void ActionButtonPressed();
	void ActionButtonReleased();

	bool bActionDown; 

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Items")
	class AWeapon* EquippedWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Items")
	class AItem* ActiveOverlappingItem;
	
	void EquippedWeapton(AWeapon* WeaponToSet);
	

	FORCEINLINE void SetActiveOverlappingItem(AItem* Item)
	{
		ActiveOverlappingItem = Item;
	}
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anims")
	bool bAttacking;

	void Attack();

	UFUNCTION(BlueprintCallable)
	void AttackEnd(); 

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims")
	class UAnimMontage* CombatMontage; 
};
