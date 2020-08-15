// Fill out your copyright notice in the Description page of Project Settings.


#include "Skill3DPreview.h"
#include "Components/SceneComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "StudyCharacter.h"
#include "CustomVariables.h"

// Sets default values
ASkill3DPreview::ASkill3DPreview()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create default subobjects
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("MeshPlaceHolder"));
	SceneRoot->SetupAttachment(RootComponent);

	SceneGround = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SceneGround"));
	SceneGround->SetupAttachment(SceneRoot);
	
	MainBody = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MainBody"));
	MainBody->SetupAttachment(SceneRoot);
	
	// Clothing System
	FaceMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Face"));
	FaceMesh->AttachToComponent(MainBody, FAttachmentTransformRules::KeepRelativeTransform, NAME_None);

	HairMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Hair"));
	HairMesh->AttachToComponent(MainBody, FAttachmentTransformRules::KeepRelativeTransform, NAME_None);
	
	HeadMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HeadGears"));
	HeadMesh->AttachToComponent(MainBody, FAttachmentTransformRules::KeepRelativeTransform, NAME_None);

	ShoulderMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ShoulderPad"));
	ShoulderMesh->AttachToComponent(MainBody, FAttachmentTransformRules::KeepRelativeTransform, NAME_None);
	
	ChestMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Chest"));
	ChestMesh->AttachToComponent(MainBody, FAttachmentTransformRules::KeepRelativeTransform, NAME_None);
	
	BackPackMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Backpack"));
	BackPackMesh->AttachToComponent(MainBody, FAttachmentTransformRules::KeepRelativeTransform, FName("Backpack"));
	
	OffWeapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("RightWeapon"));
	// OffWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, NAME_None);

	DeffWeapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("LeftWeapon"));
	// DeffWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, NAME_None);

	HandsMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Gloves"));
	HandsMesh->AttachToComponent(MainBody, FAttachmentTransformRules::KeepRelativeTransform, NAME_None);

	BeltMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Belt"));
	BeltMesh->AttachToComponent(MainBody, FAttachmentTransformRules::KeepRelativeTransform, NAME_None);

	FootsMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Boots"));
	FootsMesh->AttachToComponent(MainBody, FAttachmentTransformRules::KeepRelativeTransform, NAME_None);

	
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(MainBody);
	SpringArm->SetRelativeRotation(FRotator(-40.f, 90.f, 0.f));
	SpringArm->SetRelativeLocation(FVector(0.f, 0.f, 90.f));
	SpringArm->TargetArmLength = 800.f;

	RenderTarget = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("RenderTarget"));
	RenderTarget->SetupAttachment(SpringArm);
	RenderTarget->FOVAngle = 45.f;
	
}

// Called when the game starts or when spawned
void ASkill3DPreview::BeginPlay()
{
	Super::BeginPlay();
	
	// Force the other skeletal meshes follows the root Skeletal movement
	FaceMesh->SetMasterPoseComponent(MainBody);
	HairMesh->SetMasterPoseComponent(MainBody);
	HeadMesh->SetMasterPoseComponent(MainBody);
	ShoulderMesh->SetMasterPoseComponent(MainBody);
	ChestMesh->SetMasterPoseComponent(MainBody);
	HandsMesh->SetMasterPoseComponent(MainBody);
	BeltMesh->SetMasterPoseComponent(MainBody);
	FootsMesh->SetMasterPoseComponent(MainBody);
}

void ASkill3DPreview::UpdateArmorSet()
{
	AStudyCharacter* playerRef = Cast<AStudyCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (playerRef)
	{
		if(playerRef->GetMesh()->SkeletalMesh)
		{
			MainBody->SetSkeletalMesh(playerRef->GetMesh()->SkeletalMesh);
		}

		if(playerRef->FaceMesh->SkeletalMesh)
		{
			FaceMesh->SetSkeletalMesh(playerRef->FaceMesh->SkeletalMesh);
		}

		if(playerRef->HairMesh->SkeletalMesh)
		{
			HairMesh->SetSkeletalMesh(playerRef->HairMesh->SkeletalMesh);
		}
		
		// Update meshes if available
		if(playerRef->ArmorSetProperties[1].Mesh)
		{
			HeadMesh->SetSkeletalMesh(playerRef->ArmorSetProperties[1].Mesh);
		} else if(playerRef->HeadMesh->SkeletalMesh)
		{
			HeadMesh->SetSkeletalMesh(playerRef->HeadMesh->SkeletalMesh);
		}

		if(playerRef->ArmorSetProperties[2].Mesh)
		{
			ShoulderMesh->SetSkeletalMesh(playerRef->ArmorSetProperties[2].Mesh);
		} else if(playerRef->ShoulderMesh->SkeletalMesh)
		{
			ShoulderMesh->SetSkeletalMesh(playerRef->ShoulderMesh->SkeletalMesh);
		}

		if(playerRef->ArmorSetProperties[4].Mesh)
		{
			ChestMesh->SetSkeletalMesh(playerRef->ArmorSetProperties[4].Mesh);
		}else if(playerRef->ChestMesh->SkeletalMesh)
		{
			ChestMesh->SetSkeletalMesh(playerRef->ChestMesh->SkeletalMesh);
		}

		if(playerRef->ArmorSetProperties[6].Mesh)
		{
			HandsMesh->SetSkeletalMesh(playerRef->ArmorSetProperties[6].Mesh);
		}else if(playerRef->HandsMesh->SkeletalMesh)
		{
			HandsMesh->SetSkeletalMesh(playerRef->HandsMesh->SkeletalMesh);
		}

		if(playerRef->ArmorSetProperties[7].Mesh)
		{
			BeltMesh->SetSkeletalMesh(playerRef->ArmorSetProperties[7].Mesh);
		}else if(playerRef->BeltMesh->SkeletalMesh)
		{
			BeltMesh->SetSkeletalMesh(playerRef->BeltMesh->SkeletalMesh);
		}

		if(playerRef->ArmorSetProperties[8].Mesh)
		{
			FootsMesh->SetSkeletalMesh(playerRef->ArmorSetProperties[8].Mesh);
		}else if(playerRef->FootsMesh->SkeletalMesh)
		{
			FootsMesh->SetSkeletalMesh(playerRef->FootsMesh->SkeletalMesh);
		}

		// update offensive weapon meshes if available
		if(playerRef->ArmorSetProperties[3].Mesh)
		{
			OffWeapon->SetSkeletalMesh(playerRef->ArmorSetProperties[3].Mesh);
			OffWeapon->AttachToComponent(MainBody, FAttachmentTransformRules::SnapToTargetIncludingScale,
                playerRef->ArmorSetProperties[3].WeaponType.SocketToAttach);
		}
		
		// update Defensive weapon meshes if available
		if(playerRef->ArmorSetProperties[5].Mesh)
		{
			DeffWeapon->SetSkeletalMesh(playerRef->ArmorSetProperties[5].Mesh);
			DeffWeapon->AttachToComponent(MainBody, FAttachmentTransformRules::SnapToTargetIncludingScale,
                playerRef->ArmorSetProperties[5].WeaponType.SocketToAttach);
		}
	}
}

void ASkill3DPreview::PlaySkillAnimation(UAnimMontage* MontageToPlay)
{
	if(MainBody->GetAnimInstance())
	{
		MainBody->PlayAnimation(MontageToPlay, true);
		UE_LOG(LogTemp, Warning, TEXT("Playing Montage"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to get Animation Instance of the Mesh"));
	}
}


