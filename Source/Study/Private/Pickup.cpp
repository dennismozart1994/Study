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

// Sets default values
APickup::APickup()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// replication setup
	bReplicates = true;
	bReplicateMovement = true;
	bAlwaysRelevant = false;
	bIsNearBy = false;

	// create root component
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skeletal Mesh"));

	// bind events
	SkeletalMesh->OnBeginCursorOver.AddDynamic(this, &APickup::OnMouseOver);
	SkeletalMesh->OnEndCursorOver.AddDynamic(this, &APickup::OnMouseLeave);
	RootComponent = SkeletalMesh;

	// create mesh and static mesh component
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);

	// Create Trigger box to enable interactions with the object
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Interaction Range"));
	TriggerBox->SetupAttachment(RootComponent);
	TriggerBox->SetRelativeScale3D(FVector(4.f));
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &APickup::BeginOverlap);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &APickup::EndOverlap);

	// mesh colisions
	SkeletalMesh->SetSimulatePhysics(true);
	SkeletalMesh->BodyInstance.SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SkeletalMesh->BodyInstance.SetResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	SkeletalMesh->BodyInstance.SetResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Block);
	SkeletalMesh->BodyInstance.SetResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	SkeletalMesh->BodyInstance.SetResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);
	SkeletalMesh->BodyInstance.SetResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	SkeletalMesh->BodyInstance.SetResponseToChannel(ECollisionChannel::ECC_PhysicsBody, ECollisionResponse::ECR_Block);
	SkeletalMesh->BodyInstance.SetResponseToChannel(ECollisionChannel::ECC_Vehicle, ECollisionResponse::ECR_Ignore);
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

void APickup::OnMouseOver(UPrimitiveComponent* TouchedActor)
{
	if (bIsNearBy)
	{
		SkeletalMesh->SetRenderCustomDepth(true);
		// createOutline();
		UE_LOG(LogTemp, Log, TEXT("On Mouse Over the Object inside the interactable area"));
	}
}

void APickup::OnMouseLeave(UPrimitiveComponent* TouchedActor)
{
	SkeletalMesh->SetRenderCustomDepth(false);
	UE_LOG(LogTemp, Log, TEXT("On Mouse Leave the Object"));
}

// On Clicked on Actor event
void APickup::NotifyActorOnClicked(FKey ButtonPressed)
{
	UE_LOG(LogTemp, Log, TEXT("On Clicked has been called"));
	// If it's the client
	if (Role < ROLE_Authority)
	{
		SERVER_OnClicked(ButtonPressed);
		return;
	}

	// server func
	if (ButtonPressed == EKeys::LeftMouseButton)
	{
		// check if player is near by
		if (bIsNearBy && ActorsNearBy.Contains(UGameplayStatics::GetPlayerPawn(GetWorld(), 0)))
		{
			AStudyCharacter* CharacterRef = Cast<AStudyCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
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
		else { GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Item is not activated or player is not inside the near by array"); }
	}
	else { GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Click wasn't the Left Mouse Button"); }
}

void APickup::SERVER_OnClicked_Implementation(FKey ButtonPressed)
{
	NotifyActorOnClicked(ButtonPressed);
}

bool APickup::SERVER_OnClicked_Validate(FKey ButtonPressed)
{
	return true;
}

// begin overlap under the activation area
void APickup::BeginOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (Role < ROLE_Authority)
	{
		Server_ActivateItem(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
		return;
	}


	if (OtherComp && OtherActor)
	{
		if (OtherComp->ComponentHasTag("Player"))
		{
			bIsNearBy = true;
			if (!ActorsNearBy.Contains(OtherActor))
			{
				ActorsNearBy.Add(OtherActor);
			}
		}
	}
}

void APickup::Server_ActivateItem_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	BeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

bool APickup::Server_ActivateItem_Validate(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	return true;
}

// End Overlap the item
void APickup::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (Role < ROLE_Authority)
	{
		Server_DeactivateItem(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
		return;
	}

	if (OtherActor && OtherComp)
	{
		if (OtherComp->ComponentHasTag("Player") && ActorsNearBy.Contains(OtherActor))
		{
			ActorsNearBy.Remove(OtherActor);

			// If there's no players near by then disable the actor interaction
			if (!ActorsNearBy.IsValidIndex(0))
			{
				bIsNearBy = false;
			}
		}
	}
}

void APickup::Server_DeactivateItem_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	EndOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}

bool APickup::Server_DeactivateItem_Validate(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	return true;
}

// replication properties
void APickup::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APickup, bIsNearBy);
	DOREPLIFETIME(APickup, ActorsNearBy);
}

