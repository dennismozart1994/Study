// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CustomVariables.h"
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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Health, meta = (AllowPrivateAccess = "true"))
	class UInventoryComponent* InventoryComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Health, meta = (AllowPrivateAccess = "true"))
    class USkillTreeComponent* SkillTreeComp;
	
public:
	AStudyCharacter();

	UFUNCTION()
	void OnHealthChanged(USHealthComponent* HealthComponent, int32 Health, int32 HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	virtual void BeginPlay() override;

	virtual void Jump() override;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Replication")
	class AStudyPlayerState* CurrentPlayerState;

	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Replication")
	EWeaponType WeaponBeingUsed;

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

	// Boss Life UI bar to show when facing a new boss in the game.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> LifeBossUI;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
    TSubclassOf<class UGameplayHUD> GameplayUI;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	class UGameplayHUD* HudRef;

	// Bos UI Reference after creating the UI based on the last variable class selected on the Editor
	UPROPERTY(BlueprintReadWrite, Category = "UI")
	UUserWidget* BossUIRef;

	// Attacks replication variable
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Montages)
	TArray<class UAnimMontage*> MontagesToSort;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Montages)
	class UAnimMontage* SkillMontage;
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Skill")
	TArray<class AMasterSkill*> CurrentSkillCast;

	// Clothing System
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USkeletalMeshComponent* FaceMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USkeletalMeshComponent* HairMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USkeletalMeshComponent* HeadMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USkeletalMeshComponent* ShoulderMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USkeletalMeshComponent* ChestMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* BackPackMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USkeletalMeshComponent* OffWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USkeletalMeshComponent* DeffWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USkeletalMeshComponent* HandsMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USkeletalMeshComponent* BeltMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USkeletalMeshComponent* FootsMesh;

	/* Gameplay Functions*/
	UFUNCTION(Category = "Basics")
	bool isMovingOnGroundCheck();

	float GetSpeedMovement();

	void setCharacterSpeed();

	void createGameplayHUD();

	// Simple attack by clicking the mouse button
	UFUNCTION(Server, Reliable, WithValidation, Category = "Attacks")
	void Server_SimpleAttack();

	UFUNCTION(Server, Reliable, WithValidation, Category = "Attacks")
	void SpawnSkill(TSubclassOf<class AMasterSkill> SkillActor, int32 SlotIndex);

	UFUNCTION(NetMulticast, Reliable, WithValidation, Category = "Attacks")
	void Multicast_PlayMontage(class UAnimMontage* MontageToPlay);
	
	UFUNCTION(Server, Reliable, WithValidation, Category = "Recovery")
    void BoostPlayerStats(EBuffType Type, int32 value);

protected:

	// Basic Attacks with no Weapon
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Montages)
	TArray<class UAnimMontage*> NoWeaponBasicAttacks;

	// Basic Attacks with a Sword
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Montages)
	TArray<class UAnimMontage*> SwordBasicAttacks;

	// Basic Attacks with a Dual Blade
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Montages)
	TArray<class UAnimMontage*> DualBladeBasicAttacks;

	// Basic Attacks with an Axe Or a Blunt
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Montages)
	TArray<class UAnimMontage*> AxeOrBluntBasicAttacks;

	// Basic Attacks with a Bow
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Montages)
	TArray<class UAnimMontage*> BowBasicAttacks;

	// Basic Attacks with a Bow
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Montages)
	TArray<class UAnimMontage*> StaffAttacks;

	UPROPERTY(BlueprintReadOnly, Category = "Recovery")
	FTimerHandle _delayhandler;

	UPROPERTY(BlueprintReadWrite, Category = "Get Item Details")
	UUserWidget* ItemInfoRef;

	UFUNCTION(Server, Reliable, WithValidation, Category = "Recovery")
	void RecoverStamina();

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

	FORCEINLINE class UUserWidget*  GetItemWidgetRef() const { return ItemInfoRef; }

	FORCEINLINE class USHealthComponent*  GetHealthComponent() const { return HealthComp; }

	FORCEINLINE class USkillTreeComponent*  GetSkillTreeComponent() const { return SkillTreeComp; }
};

