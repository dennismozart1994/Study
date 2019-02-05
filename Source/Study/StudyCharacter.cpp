// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "StudyCharacter.h"
#include "StudyPlayerState.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "SHealthComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"
#include "GameFramework/SpringArmComponent.h"

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
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

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
}

///////////////////////////////////////// Functions /////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
///////////////////////////////////////// replication /////////////////////////////////////
void AStudyCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
 {
     Super::GetLifetimeReplicatedProps(OutLifetimeProps);
 
     DOREPLIFETIME(AStudyCharacter, ArmorSet);
	 DOREPLIFETIME(AStudyCharacter, ArmorSetProperties);
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
}

//////////////////////////////////////// Setup Inputs ///////////////////////////////////////////
void AStudyCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

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
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AStudyCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

///////////////////////////////////////// Character Movement ///////////////////////////////////
void AStudyCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AStudyCharacter::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) )
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
