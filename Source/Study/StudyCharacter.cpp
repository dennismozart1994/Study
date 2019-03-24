// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "StudyCharacter.h"
#include "StudyPlayerState.h"
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

//////////////////////////////////////////////////////////////////////////
// AStudyCharacter Constructor
AStudyCharacter::AStudyCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	GetCharacterMovement()->MaxWalkSpeed = 200.f;
	
	// replication setup
	bReplicates = true;
	bReplicateMovement = true;

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
	OffWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, NAME_None);

	DeffWeapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon2"));
	DeffWeapon->SetIsReplicated(true);
	DeffWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, NAME_None);

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
	 DOREPLIFETIME(AStudyCharacter, NoWeaponBasicAttacks);
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

	CurrentPlayerState = Cast<AStudyPlayerState>(GetPlayerState());
	if(CurrentPlayerState)
	{
		GetCharacterMovement()->MaxWalkSpeed = float(CurrentPlayerState->CharacterStats.Speed);
	}

	// Create Life Boss Reference Widget but don´t add into the viewport
	BossUIRef = CreateWidget<UUserWidget>(UGameplayStatics::GetPlayerController(GetWorld(), 0), LifeBossUI, NAME_None);
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

void AStudyCharacter::Server_SimpleAttack_Implementation()
{
	if (Role == ROLE_Authority)
	{
		TArray<UAnimMontage*> MontagesToSort;
		int32 Range = 0;

		if (bCanAttack && !bIsReceivingDamage)
		{
			UE_LOG(LogTemp, Log, TEXT("Server is executing simple Attack"));

			// check if use basic attacks animations or the animations for the weapon
			if (WeaponBeingUsed == EWeaponType::WT_None)
			{
				Range = NoWeaponBasicAttacks.Num() - 1;
				MontagesToSort = NoWeaponBasicAttacks;
				UE_LOG(LogTemp, Log, TEXT("No Weapon Basic Attacks selected"));
			}
			else
			{
				Range = ArmorSetProperties[3].WeaponType.BasicAttacks.Num() - 1;
				MontagesToSort = ArmorSetProperties[3].WeaponType.BasicAttacks;
			}

			int32 RandomIndex = UKismetMathLibrary::RandomIntegerInRange(0, Range);

			if (MontagesToSort.IsValidIndex(RandomIndex))
			{
				bCanAttack = false;
				Multicast_PlayMontage(MontagesToSort[RandomIndex]);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Montage is not valid"));
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
	PlayAnimMontage(MontageToPlay, 1.f, FName("None"));
	UE_LOG(LogTemp, Log, TEXT("Played the Montage"));
}

bool AStudyCharacter::Multicast_PlayMontage_Validate(UAnimMontage* MontageToPlay)
{
	return true;
}

// Drop item on UWorld
void AStudyCharacter::DropItemOnWorld_Implementation(TSubclassOf<AActor> PickupClass, FTransform Location, ESlotType SlotType, int32 SlotID)
{
	UWorld* World = GetWorld();
	if (HasAuthority() && PickupClass != NULL && World)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		World->SpawnActor<APickup>(PickupClass, Location, SpawnParams);
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
