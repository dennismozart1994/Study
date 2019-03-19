// Fill out your copyright notice in the Description page of Project Settings.

#include "Pickup.h"
#include "StudyPC.h"
#include "StudyCharacter.h"
#include "StudyPlayerState.h"
#include "Engine/DataTable.h"
#include "Engine/Engine.h"
#include "Engine/Classes/Kismet/GameplayStatics.h"
#include "Engine/Classes/Kismet/KismetSystemLibrary.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
APickup::APickup()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// replication setup
	bReplicates = true;
	bReplicateMovement = true;
	bAlwaysRelevant = false;

	// create root component
	RootC = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = RootC;

	// create trigger box and bind begin overlap function
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Colision"));
	TriggerBox->SetupAttachment(RootComponent);
	TriggerBox->SetRelativeScale3D(FVector(1.f, 1.f, 1.f));
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &APickup::BeginOverlap);

	// create mesh and static mesh component
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skeletal Mesh"));
	SkeletalMesh->SetupAttachment(RootComponent);

	// mesh colisions
	SkeletalMesh->SetSimulatePhysics(true);
	SkeletalMesh->BodyInstance.SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SkeletalMesh->BodyInstance.SetResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Ignore);
	SkeletalMesh->BodyInstance.SetResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Block);
	SkeletalMesh->BodyInstance.SetResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	SkeletalMesh->BodyInstance.SetResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);
	SkeletalMesh->BodyInstance.SetResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	SkeletalMesh->BodyInstance.SetResponseToChannel(ECollisionChannel::ECC_PhysicsBody, ECollisionResponse::ECR_Block);
	SkeletalMesh->BodyInstance.SetResponseToChannel(ECollisionChannel::ECC_Vehicle, ECollisionResponse::ECR_Block);
	SkeletalMesh->BodyInstance.SetResponseToChannel(ECollisionChannel::ECC_Destructible, ECollisionResponse::ECR_Block);

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
	// ItemIndexDataTable = FName(*(UKismetSystemLibrary::GetClassDisplayName(UGameplayStatics::GetObjectClass(this))));
}

void APickup::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	// If it's the client
	if (Role < ROLE_Authority)
	{
		SERVER_BeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
		return;
	}

	// server func
	if (OtherComp && OtherComp->ComponentHasTag("Player"))
	{
		AStudyCharacter* CharacterRef = Cast<AStudyCharacter>(OtherActor);
		if (CharacterRef)
		{
			AStudyPlayerState* PlayerStateRef = Cast<AStudyPlayerState>(CharacterRef->GetPlayerState());
			if (PlayerStateRef)
			{
				APawn* PawnRef = PlayerStateRef->GetPawn();
				if (PawnRef)
				{
					AStudyPC* ControllerRef = Cast<AStudyPC>(PawnRef->GetController());
					if (ControllerRef)
					{
						for (int i = 0; i < ControllerRef->Inventory.Num(); i++)
						{
							// check if the current item is the last and is valid
							if (UKismetSystemLibrary::IsValidClass(ControllerRef->Inventory[i]) && i == 35)
							{
								GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Inventory is full");
							}
							// if not the last item of the array
							// check if it's not a valid class on the index
							// if it's a valid class then do nothing and go to the next step
							// if it's not a valid class, add this item to inventory into that array index
							else if (!UKismetSystemLibrary::IsValidClass(ControllerRef->Inventory[i]))
							{
								GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, "Added to Inventory");
								ControllerRef->Inventory[i] = this->GetClass();
								static const FString ContextCurrent(TEXT("Current Item Details"));
								ControllerRef->InventoryItems[i] = *(ItemDataTable->FindRow<FItemDetailsDataTable>(ItemIndexDataTable, ContextCurrent, true));
								Destroy();
								break;
							}
						}
					}
					else { GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Controller not found"); }
				}
				else { GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Error Casting to Private Pawn"); }
			}
			else { GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Error Casting tom PlayerState"); }
		}
		else { GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Error Casting to the Character"); }
	}
	else { GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "No Player Overlapped"); }
}

void APickup::SERVER_BeginOverlap_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	BeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

bool APickup::SERVER_BeginOverlap_Validate(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	return true;
}


