// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CustomVariables.h"
#include "Components/SkeletalMeshComponent.h"
#include "StudyCharacter.generated.h"

class AWeaponToSpawn;

UCLASS(config=Game)
class AStudyCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Health, meta = (AllowPrivateAccess = "true"))
	class USHealthComponent* HealthComp;
	
public:
	AStudyCharacter();

	UFUNCTION()
	void OnHealthChanged(USHealthComponent* HealthComponent, int32 Health, int32 HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	virtual void BeginPlay() override;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Replication")
	class AStudyPlayerState* CurrentPlayerState;

	// Gameplay Variables
	UPROPERTY(BlueprintReadWrite, Category = "Reactions")
	bool bIsReceivingDamage;

	UPROPERTY(BlueprintReadWrite, Category = "Reactions")
	bool bCanAttack;

	UPROPERTY(BlueprintReadWrite, Category = "Reactions")
	bool bCanWalk;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category="Gameplay")
	TArray<TSubclassOf<AActor>> ArmorSet;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	TArray<FItemDetailsDataTable> ArmorSetProperties;

	// Weapons Reference
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = "Gameplay")
	AWeaponToSpawn* Weapon1;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = "Gameplay")
	AWeaponToSpawn* Weapon2;

	// Boss Life UI bar to show when facing a new boss in the game.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> LifeBossUI;

	// Bos UI Reference after creating the UI based on the last variable class selected on the Editor
	UPROPERTY(BlueprintReadWrite, Category = "UI")
	UUserWidget* BossUIRef;

	// Basic Attacks with no Weapon
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Montages)
	TArray<UAnimMontage*> NoWeaponBasicAttacks;

	// Clothing System
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* HeadMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* ChestMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* HandsMesh;

	UPROPERTY( EditAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* LegsMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* FootsMesh;

	/* Gameplay Functions*/
	UFUNCTION(Category = "Basics")
	bool isMovingOnGroundCheck();

	float GetSpeedMovement();

	// Simple attack by clicking the mouse button
	UFUNCTION(Server, Reliable, WithValidation, Category = "Attacks")
	void Server_SimpleAttack();

	UFUNCTION(NetMulticast, UnReliable, WithValidation, Category = "Attacks")
	void Multicast_PlayMontage(UAnimMontage* MontageToPlay);

	// Spawn Weapon and attach to the user
	UFUNCTION(BlueprintCallable, Category = "Attachs")
	void SpawnWeapon(int32 SlotIndex, FItemDetailsDataTable ItemDetails);

	UFUNCTION(Server, Reliable, WithValidation, Category = "Inventory")
	void DropItemOnWorld(TSubclassOf<AActor> PickupClass, FTransform Location);

protected:
	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

