// Fill out your copyright notice in the Description page of Project Settings.

#include "Pickup.h"
#include "StudyPC.h"
#include "StudyCharacter.h"
#include "StudyPlayerState.h"
#include "Engine/DataTable.h"
#include "Engine/Engine.h"
#include "Engine/Classes/Kismet/GameplayStatics.h"
#include "Engine/Classes/Kismet/KismetSystemLibrary.h"
#include "Engine/Classes/Materials/MaterialInstanceDynamic.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Runtime/Engine/Public/TimerManager.h"

// Sets default values
APickup::APickup()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// replication setup
	bReplicates = true;
	bReplicateMovement = true;
	bAlwaysRelevant = false;
	NetUpdateFrequency = 100.f;

	// create root component
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skeletal Mesh"));
	RootComponent = SkeletalMesh;

	// bind events
	SkeletalMesh->OnBeginCursorOver.AddDynamic(this, &APickup::OnMouseOver);
	SkeletalMesh->OnEndCursorOver.AddDynamic(this, &APickup::OnMouseLeave);

	// create mesh and static mesh component
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
	Mesh->OnBeginCursorOver.AddDynamic(this, &APickup::OnMouseOver);
	Mesh->OnEndCursorOver.AddDynamic(this, &APickup::OnMouseLeave);
	Mesh->BodyInstance.SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Mesh->BodyInstance.SetResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Block);
	Mesh->BodyInstance.SetResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Ignore);

	// Create Trigger box to enable interactions with the object
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Interaction Range"));
	TriggerBox->SetupAttachment(RootComponent);
	TriggerBox->BodyInstance.SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->BodyInstance.SetResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Block);
	TriggerBox->BodyInstance.SetResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Overlap);
	TriggerBox->SetRelativeScale3D(FVector(2.f));
	TriggerBox->OnEndCursorOver.AddDynamic(this, &APickup::OnMouseLeave);
	TriggerBox->OnBeginCursorOver.AddDynamic(this, &APickup::OnMouseOver);
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &APickup::BeginOverlap);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &APickup::EndOverlap);

	// mesh colisions
	SkeletalMesh->SetSimulatePhysics(true);
	SkeletalMesh->BodyInstance.SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SkeletalMesh->BodyInstance.SetResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Ignore);
	SkeletalMesh->BodyInstance.SetResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Block);
	SkeletalMesh->BodyInstance.SetResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	SkeletalMesh->BodyInstance.SetResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);
	SkeletalMesh->BodyInstance.SetResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	SkeletalMesh->BodyInstance.SetResponseToChannel(ECollisionChannel::ECC_PhysicsBody, ECollisionResponse::ECR_Ignore);
	SkeletalMesh->BodyInstance.SetResponseToChannel(ECollisionChannel::ECC_Vehicle, ECollisionResponse::ECR_Ignore);
	SkeletalMesh->BodyInstance.SetResponseToChannel(ECollisionChannel::ECC_Destructible, ECollisionResponse::ECR_Ignore);

	// load data table
	static ConstructorHelpers::FObjectFinder<UDataTable> PickupDataTableObject(TEXT("DataTable'/Game/DataTables/Pickups.Pickups'"));
	if (PickupDataTableObject.Succeeded())
	{
		ItemDataTable = PickupDataTableObject.Object;
	}

}

// Called when the game starts or when spawned
void APickup::BeginPlay()
{
	Super::BeginPlay();
	GetWorld()->GetTimerManager().SetTimer(_delayhandler, this, &APickup::deactivatePhysics, 4.f, false);
}

void APickup::deactivatePhysics()
{
	SkeletalMesh->SetSimulatePhysics(false);
	GetWorld()->GetTimerManager().ClearTimer(_delayhandler);
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}

FItemDetailsDataTable APickup::getItemInfo()
{
	if (ItemDataTable)
	{
		static const FString ContextCurrent(TEXT("Current Item Details"));
		return *(ItemDataTable->FindRow<FItemDetailsDataTable>(ItemIndexDataTable, ContextCurrent, true));
	}
	else
	{
		return FItemDetailsDataTable();
	}
}

void APickup::OnMouseOver(UPrimitiveComponent* TouchedActor)
{
	SkeletalMesh->SetRenderCustomDepth(true);
	UE_LOG(LogTemp, Log, TEXT("On Mouse Over the Object"));
}

void APickup::OnMouseLeave(UPrimitiveComponent* TouchedActor)
{
	SkeletalMesh->SetRenderCustomDepth(false);
	UE_LOG(LogTemp, Log, TEXT("On Mouse Leave the Object"));
}

// begin overlap under the activation area
void APickup::BeginOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherComp && OtherActor)
	{
		if (!OtherComp->ComponentHasTag("Player"))
		{
			return;
		}

		if (OtherComp->ComponentHasTag("Player"))
		{
			UE_LOG(LogTemp, Warning, TEXT("Actor %s inside the bounds"), *UKismetSystemLibrary::GetDisplayName(OtherActor));
			SkeletalMesh->SetRenderCustomDepth(true);
		}
	}
}

// End Overlap the item
void APickup::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherComp && OtherActor)
	{
		if (!OtherComp->ComponentHasTag("Player"))
		{
			return;
		}

		if (OtherComp->ComponentHasTag("Player"))
		{
			SkeletalMesh->SetRenderCustomDepth(false);
		}
	}
}
