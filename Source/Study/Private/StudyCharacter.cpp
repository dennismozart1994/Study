// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "StudyCharacter.h"
#include "StudyPlayerState.h"
#include "StudyPC.h"
#include "Pickup.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "SHealthComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"
#include "GameFramework/SpringArmComponent.h"
#include "Blueprint/UserWidget.h"
#include "Runtime/Engine/Public/TimerManager.h"

//////////////////////////////////////////////////////////////////////////
// AStudyCharacter Constructor
AStudyCharacter::AStudyCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	GetCharacterMovement()->MaxWalkSpeed = 400.f;
	
	// replication setup
	bReplicates = true;
	SetReplicatingMovement(true);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetRelativeRotation(FRotator(-40.f, 0.f, 0.f)); // The camera Rotation (Top Down Angle)	
	CameraBoom->TargetArmLength = 1200.0f; // The camera follows at this distance behind the character	
	CameraBoom->bDoCollisionTest = true;
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	CameraBoom->bInheritPitch = false;
	CameraBoom->bInheritYaw = true;
	CameraBoom->bInheritRoll = true;
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->bEnableCameraRotationLag = true;
	CameraBoom->CameraLagSpeed = 30.f;
	CameraBoom->CameraRotationLagSpeed = 50.f;
	CameraBoom->ProbeChannel = ECC_Visibility;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Health Component
	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComponent"));

	// Clothing System
	HeadMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Head"));
	HeadMesh->SetIsReplicated(true);
	HeadMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, NAME_None);

	ChestMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Chest"));
	ChestMesh->SetIsReplicated(true);
	ChestMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, NAME_None);

	OffWeapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon1"));
	OffWeapon->SetIsReplicated(true);
	// OffWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, NAME_None);

	DeffWeapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon2"));
	DeffWeapon->SetIsReplicated(true);
	// DeffWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, NAME_None);

	HandsMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Gloves"));
	HandsMesh->SetIsReplicated(true);
	HandsMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, NAME_None);

	LegsMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Trousers"));
	LegsMesh->SetIsReplicated(true);
	LegsMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, NAME_None);

	FootsMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Boots"));
	FootsMesh->SetIsReplicated(true);
	FootsMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, NAME_None);

	// Setup Gameplay Variables
	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
	bIsReceivingDamage = false;
	bCanAttack = true;
	bCanWalk = true;
	WeaponBeingUsed = EWeaponType::WT_None;
}
//////////////////////////////////////////////////////////////////////////
///////////////////////////////////////// replication /////////////////////////////////////
void AStudyCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
 {
     Super::GetLifetimeReplicatedProps(OutLifetimeProps);
 
     DOREPLIFETIME(AStudyCharacter, ArmorSet);
	 DOREPLIFETIME(AStudyCharacter, ArmorSetProperties);
	 DOREPLIFETIME(AStudyCharacter, MontagesToSort);
	 DOREPLIFETIME(AStudyCharacter, WeaponBeingUsed);
 }
 
//////////////////////////////////// Native events ////////////////////////////////////////////
void AStudyCharacter::BeginPlay()
{
	Super::BeginPlay();

	HealthComp->OnHealthChanged.AddDynamic(this, &AStudyCharacter::OnHealthChanged);

	// Force the other skeletal meshes follows the root Skeletal movement
	HeadMesh->SetMasterPoseComponent(GetMesh());
	ChestMesh->SetMasterPoseComponent(GetMesh());
	HandsMesh->SetMasterPoseComponent(GetMesh());
	LegsMesh->SetMasterPoseComponent(GetMesh());
	FootsMesh->SetMasterPoseComponent(GetMesh());

	setCharacterSpeed();

	// Create Life Boss Reference Widget but don´t add into the viewport
	AController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PlayerController)
	{
		if (PlayerController->IsLocalController())
		{
			BossUIRef = CreateWidget<UUserWidget>(UGameplayStatics::GetPlayerController(GetWorld(), 0), LifeBossUI, NAME_None);
		}
	}
}

//////////////////////////////////////// Setup Inputs ///////////////////////////////////////////
void AStudyCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("SimpleAttack", IE_Pressed, this, &AStudyCharacter::Server_SimpleAttack);

	PlayerInputComponent->BindAxis("MoveForward", this, &AStudyCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AStudyCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AStudyCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AStudyCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AStudyCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AStudyCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AStudyCharacter::OnResetVR);
}


void AStudyCharacter::OnHealthChanged(USHealthComponent* HealthComponent, int32 Health, int32 HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	AStudyPlayerState* PlayerStateRef = Cast<AStudyPlayerState>(GetPlayerState());
	
	// Died
	if (PlayerStateRef->CharacterStats.ActualLife <= 0 && PlayerStateRef->bIsAlive)
	{
		PlayerStateRef->bIsAlive = false;
		GetMovementComponent()->StopMovementImmediately();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// DetachFromControllerPendingDestroy();
		// SetLifeSpan(10.f);
		UE_LOG(LogTemp, Log, TEXT("Character Died"));
	}
	else
	{
		PlayerStateRef->bIsAlive = true;
		UE_LOG(LogTemp, Log, TEXT("Health Changed: %s"), *FString::SanitizeFloat(float(PlayerStateRef->CharacterStats.ActualLife)));
	}
}

bool AStudyCharacter::isMovingOnGroundCheck()
{
	return GetCharacterMovement()->IsMovingOnGround();
}

float AStudyCharacter::GetSpeedMovement()
{
	return GetCharacterMovement()->MaxWalkSpeed;
}

void AStudyCharacter::setCharacterSpeed()
{
	CurrentPlayerState = Cast<AStudyPlayerState>(GetPlayerState());
	if (CurrentPlayerState)
	{
		GetCharacterMovement()->MaxWalkSpeed = float(CurrentPlayerState->CharacterStats.Speed);
		UE_LOG(LogTemp, Log, TEXT("Successfully casted and added the current speed on the character"));
		GetWorld()->GetTimerManager().ClearTimer(_delayhandler);
		GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ERROR trying to cast to PlayerState to set the current speed on the character"));
		GetWorld()->GetTimerManager().SetTimer(_delayhandler, this, &AStudyCharacter::setCharacterSpeed, 1.f, false);
	}
}

void AStudyCharacter::PickupItem(APickup * Item)
{
	AStudyPlayerState* PSRef = Cast<AStudyPlayerState>(GetPlayerState());
	if (PSRef)
	{
		Server_PickupItem(Item, PSRef);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Error trying to Cast to Player State"));
	}
}

void AStudyCharacter::Server_PickupItem_Implementation(APickup* Item, AStudyPlayerState* PSRef)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		APawn* PPawn = PSRef->GetPawn();
		if (PPawn)
		{
			AStudyPC* ControllerRef = Cast<AStudyPC>(PPawn->GetController());
			if (ControllerRef)
			{
				if (Item)
				{
					for (int i = 0; i < ControllerRef->Inventory.Num(); i++)
					{
						// check if the current item is the last and is valid
						if (UKismetSystemLibrary::IsValidClass(ControllerRef->Inventory[i]) && i == 35)
						{
							UE_LOG(LogTemp, Warning, TEXT("Character can't pickup Item because his Inventory is full"));
						}
						// if not the last item of the array
						// check if it's not a valid class on the index
						// if it's a valid class then do nothing and go to the next step
						// if it's not a valid class, add this item to inventory into that array index
						else if (!UKismetSystemLibrary::IsValidClass(ControllerRef->Inventory[i]))
						{
							ControllerRef->Inventory[i] = Item->GetClass();
							ControllerRef->InventoryItems[i] = Item->getItemInfo();
							FlushNetDormancy();
							UE_LOG(LogTemp, Log, TEXT("Character pickup Item and added to Inventory"));
							Item->Destroy();
							break;
						}
					}
				}
				else { UE_LOG(LogTemp, Error, TEXT("Error trying to Cast to Pickup Item")); }
			}
			else { UE_LOG(LogTemp, Error, TEXT("Error trying to Cast to Player Controller")); }
		}
		else { UE_LOG(LogTemp, Error, TEXT("Error trying to Cast to Player Pawn")); }
	}
}

bool AStudyCharacter::Server_PickupItem_Validate(APickup* Item, AStudyPlayerState* PSRef)
{
	return true;
}

void AStudyCharacter::Server_SimpleAttack_Implementation()
{
	UWorld* World = GetWorld();

	AStudyPlayerState* PSRef = Cast<AStudyPlayerState>(GetPlayerState());

	// otherwise execute a simple attack
	if (GetLocalRole() == ROLE_Authority)
	{
		int32 Range = 0;
		
		if (bCanAttack && !bIsReceivingDamage && PSRef)
		{
			UE_LOG(LogTemp, Log, TEXT("Server is executing simple Attack"));
			if (PSRef->CharacterStats.ActualStamina >= 20)
			{
				// check if use basic attacks animations or the animations for the weapon
				switch (WeaponBeingUsed)
				{
					// Magician Weapon
				case EWeaponType::WT_Staff:
					Range = StaffAttacks.Num() - 1;
					MontagesToSort = StaffAttacks;
					UE_LOG(LogTemp, Log, TEXT("Staff Basic Attacks selected"));
					break;

					// Sword Weapon
				case EWeaponType::WT_Sword:
					Range = SwordBasicAttacks.Num() - 1;
					MontagesToSort = SwordBasicAttacks;
					UE_LOG(LogTemp, Log, TEXT("Sword Basic Attacks selected"));
					break;

					// Axe or Blunt Weapon
				case EWeaponType::WT_Axe_OR_Blunt:
					Range = AxeOrBluntBasicAttacks.Num() - 1;
					MontagesToSort = AxeOrBluntBasicAttacks;
					UE_LOG(LogTemp, Log, TEXT("Axe or Blunt Basic Attacks selected"));
					break;

					// Dual Blade Weapon
				case EWeaponType::WT_DualBlade:
					Range = DualBladeBasicAttacks.Num() - 1;
					MontagesToSort = DualBladeBasicAttacks;
					UE_LOG(LogTemp, Log, TEXT("Dual Blade Basic Attacks selected"));
					break;

					// Bow and Arrow Weapon
				case EWeaponType::WT_Bow:
					Range = BowBasicAttacks.Num() - 1;
					MontagesToSort = BowBasicAttacks;
					UE_LOG(LogTemp, Log, TEXT("Bow Basic Attacks selected"));
					break;

					// No Weapon was seletected
				default:
					Range = NoWeaponBasicAttacks.Num() - 1;
					MontagesToSort = NoWeaponBasicAttacks;
					UE_LOG(LogTemp, Log, TEXT("No Weapon Basic Attacks selected"));
					break;
				}

				int32 RandomIndex = UKismetMathLibrary::RandomIntegerInRange(0, Range);

				if (MontagesToSort.IsValidIndex(Range) && World)
				{
					bCanAttack = false;
					Multicast_PlayMontage(MontagesToSort[RandomIndex]);
					PSRef->CharacterStats.ActualStamina = FMath::Clamp(PSRef->CharacterStats.ActualStamina - 20, 0, PSRef->CharacterStats.FullStamina);
					World->GetTimerManager().SetTimer(_delayhandler, this, &AStudyCharacter::RecoverStamina, 2.5f, false);
					UE_LOG(LogTemp, Log, TEXT("Simple attack performed and started timer"));
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("Montage is not valid, Index %i"), RandomIndex);
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Character doesn't have enough stamina to perform attack"));
			}

		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("Player Can't attack"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Not the server trying to play the animation"));
	}
}

bool AStudyCharacter::Server_SimpleAttack_Validate()
{
	return true;
}

// Play on All clients(Including the Server) the animation
void AStudyCharacter::Multicast_PlayMontage_Implementation(UAnimMontage* MontageToPlay)
{
	AStudyPlayerState* PSRef = Cast<AStudyPlayerState>(GetPlayerState());
	if (PSRef)
	{
		PlayAnimMontage(MontageToPlay, PSRef->CharacterStats.Speed / 200, FName("None"));
		UE_LOG(LogTemp, Log, TEXT("Played the Montage"));
	}
}

bool AStudyCharacter::Multicast_PlayMontage_Validate(UAnimMontage* MontageToPlay)
{
	return true;
}

// Drop item on UWorld
void AStudyCharacter::DropItemOnWorld_Implementation(TSubclassOf<AActor> PickupClass, FTransform Location, ESlotType SlotType, int32 SlotID)
{
	UWorld* World = GetWorld();
	if (GetLocalRole() == ROLE_Authority && PickupClass != NULL && World)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		World->SpawnActor<AActor>(PickupClass, Location, SpawnParams);
		if (SlotType == ESlotType::ST_ArmorSet && SlotID == 3)
		{
			// Don't change if there's another dual blade on the second hand
			if (ArmorSetProperties[5].WeaponType.WeaponType != EWeaponType::WT_DualBlade)
			{
				WeaponBeingUsed = EWeaponType::WT_None;
			}
		}

		if (SlotType == ESlotType::ST_ArmorSet && SlotID == 5)
		{
			// only change if there's no dual blade on the other hand
			if (ArmorSetProperties[5].WeaponType.WeaponType == EWeaponType::WT_DualBlade && ArmorSetProperties[3].WeaponType.WeaponType == EWeaponType::WT_None)
			{
				WeaponBeingUsed = EWeaponType::WT_None;
			}
		}
		UE_LOG(LogTemp, Log, TEXT("The item %s was Dropped"), *UKismetSystemLibrary::GetClassDisplayName(PickupClass));
	}
}

bool AStudyCharacter::DropItemOnWorld_Validate(TSubclassOf<AActor> PickupClass, FTransform Location, ESlotType SlotType, int32 SlotID)
{
	return true;
}

void AStudyCharacter::RecoverStamina_Implementation()
{
	UWorld* World = GetWorld();
	AStudyPlayerState* PSRef = Cast<AStudyPlayerState>(GetPlayerState());

	if (GetLocalRole() == ROLE_Authority && PSRef && World)
	{
		if (!bIsReceivingDamage && bCanAttack && PSRef->CharacterStats.ActualStamina < PSRef->CharacterStats.FullStamina)
		{
			PSRef->CharacterStats.ActualStamina = FMath::Clamp(PSRef->CharacterStats.ActualStamina + 5, 0, PSRef->CharacterStats.FullStamina);
			World->GetTimerManager().SetTimer(_delayhandler, this, &AStudyCharacter::RecoverStamina, 1.f, false);
			UE_LOG(LogTemp, Log, TEXT("Recovered 5 points of stamina"));
		}
		else if(PSRef->CharacterStats.ActualStamina + 5 >= PSRef->CharacterStats.FullStamina)
		{
			World->GetTimerManager().ClearTimer(_delayhandler);
			World->GetTimerManager().ClearAllTimersForObject(this);
			UE_LOG(LogTemp, Log, TEXT("No need to recover stamina, Clearing out the timers"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Can't recover stamina because your attacking or taking damage, will retry"));
			World->GetTimerManager().SetTimer(_delayhandler, this, &AStudyCharacter::RecoverStamina, 1.f, false);
		}
	}
	else
	{
		RecoverStamina();
		UE_LOG(LogTemp, Error, TEXT("Not the Server or Player State is Invalid or World is not valid"));
	}
}

bool AStudyCharacter::RecoverStamina_Validate()
{
	return true;
}

void AStudyCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AStudyCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
		Jump();
}

void AStudyCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
		StopJumping();
}

//////////////////////////////////////////// Camera effects ///////////////////////////////////////
void AStudyCharacter::TurnAtRate(float Rate)
{
	AStudyPlayerState* StateRef = Cast<AStudyPlayerState>(GetPlayerState());
	if (StateRef)
	{
		if (StateRef->CharacterStats.ActualLife > 0 && bCanWalk)
		{
			// calculate delta for this frame from the rate information
			AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
		}
	}
}

void AStudyCharacter::LookUpAtRate(float Rate)
{
	AStudyPlayerState* StateRef = Cast<AStudyPlayerState>(GetPlayerState());
	if (StateRef)
	{
		if (StateRef->CharacterStats.ActualLife > 0 && bCanWalk)
		{
			// calculate delta for this frame from the rate information
			AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
		}
	}
}

///////////////////////////////////////// Character Movement ///////////////////////////////////
void AStudyCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		AStudyPlayerState* StateRef = Cast<AStudyPlayerState>(GetPlayerState());
		if (StateRef)
		{
			if (StateRef->CharacterStats.ActualLife > 0 && bCanWalk)
			{
				// find out which way is forward
				const FRotator Rotation = Controller->GetControlRotation();
				const FRotator YawRotation(0, Rotation.Yaw, 0);

				// get forward vector
				const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
				AddMovementInput(Direction, Value);
				bCanAttack = true;
			}
		}
	}
}

void AStudyCharacter::MoveRight(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		AStudyPlayerState* StateRef = Cast<AStudyPlayerState>(GetPlayerState());
		if (StateRef)
		{
			if (StateRef->CharacterStats.ActualLife > 0 && bCanWalk)
			{
				// find out which way is right
				const FRotator Rotation = Controller->GetControlRotation();
				const FRotator YawRotation(0, Rotation.Yaw, 0);

				// get right vector 
				const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
				// add movement in that direction
				AddMovementInput(Direction, Value);
			}
		}
	}
}
