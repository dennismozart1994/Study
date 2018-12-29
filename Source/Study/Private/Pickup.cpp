// Fill out your copyright notice in the Description page of Project Settings.

#include "Pickup.h"
#include "StudyPC.h"
#include "StudyCharacter.h"
#include "Engine/Classes/Kismet/GameplayStatics.h"
#include "Engine/Classes/Kismet/KismetSystemLibrary.h"

// Sets default values
APickup::APickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootC = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = RootC;
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Colision"));
	TriggerBox->SetupAttachment(RootComponent);
	TriggerBox->SetRelativeScale3D(FVector(1.f, 1.f, 1.f));
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &APickup::BeginOverlap);
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
}

// Called when the game starts or when spawned
void APickup::BeginPlay()
{
	Super::BeginPlay();
	
}

void APickup::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if(OtherComp && OtherComp->ComponentHasTag("Player"))
	{
		AStudyPC* ControllerRef = Cast<AStudyPC>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
		if(ControllerRef)
		{
			for(int i=0; i<ControllerRef->Inventory.Num(); i++)
			{
				// check if the current item is the last and is valid
				if(UKismetSystemLibrary::IsValidClass(ControllerRef->Inventory[i]) && i == 35)
				{
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Inventory is full");
				}
				// if not the last item of the array
				// check if it's not a valid class on the index
				// if it's a valid class then do nothing and go to the next step
				// if it's not a valid class, add this item to inventory into that array index
				else if(!UKismetSystemLibrary::IsValidClass(ControllerRef->Inventory[i]))
				{
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, "Added to Inventory");
					ControllerRef->Inventory[i] = this->GetClass();
					Destroy();
					break;
				}
			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Controller not found");
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "No Player Overlapped");
	}
}


