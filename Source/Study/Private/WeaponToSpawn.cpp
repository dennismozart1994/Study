// Fill out your copyright notice in the Description page of Project Settings.

#include "WeaponToSpawn.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"	

// Sets default values
AWeaponToSpawn::AWeaponToSpawn()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// replication setup
	bReplicates = true;																																																																																																																																																																							
	bReplicateMovement = false;
	bAlwaysRelevant = false;

	// create root component
	RootC = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = RootC;

	// create mesh and static mesh component
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skeletal Mesh"));
	SkeletalMesh->SetupAttachment(RootComponent);

	// Mesh Colisions
	SkeletalMesh->SetSimulatePhysics(false);
	SkeletalMesh->BodyInstance.SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SkeletalMesh->BodyInstance.SetResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Ignore);
	SkeletalMesh->BodyInstance.SetResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	SkeletalMesh->BodyInstance.SetResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Ignore);
	SkeletalMesh->BodyInstance.SetResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);
	SkeletalMesh->BodyInstance.SetResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	SkeletalMesh->BodyInstance.SetResponseToChannel(ECollisionChannel::ECC_PhysicsBody, ECollisionResponse::ECR_Ignore);
	SkeletalMesh->BodyInstance.SetResponseToChannel(ECollisionChannel::ECC_Vehicle, ECollisionResponse::ECR_Block);
	SkeletalMesh->BodyInstance.SetResponseToChannel(ECollisionChannel::ECC_Destructible, ECollisionResponse::ECR_Ignore);
}

void AWeaponToSpawn::SetNewMesh(USkeletalMesh * newSkeletalMesh, UStaticMesh * newStaticMesh)
{
	if (newSkeletalMesh != NULL)
	{
		SkeletalMesh->SetSkeletalMesh(newSkeletalMesh, true);
	}

	if (newStaticMesh != NULL)
	{
		Mesh->SetStaticMesh(newStaticMesh);
	}
}
