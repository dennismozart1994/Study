// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"
#include "Blueprint/UserWidget.h"
#include "StudyCharacter.h"
#include "StudyPlayerState.h"
#include "StudyPC.h"
#include "Pickup.h"
#include "PickupInfo.h"
#include "Item3DDescription.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "InventoryDragDropOperation.h"
#include "WidgetBlueprintLibrary.h"
#include "Blueprint/UserWidget.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	SetIsReplicatedByDefault(true);
	bFoundSomething = false;
}


// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// Only store the owner in the server side
	if (GetOwnerRole() == ROLE_Authority || (GetOwnerRole() == ROLE_Authority && GetOwner()->GetRemoteRole() < ROLE_AutonomousProxy))
	{
		MyOwner = GetOwner();
	}
}

void UInventoryComponent::PickupClosestItem()
{
	bFoundSomething = false;
	AStudyCharacter* CharacterRef = Cast<AStudyCharacter>(GetOwner());
	// If there's a character as a owner
	if(CharacterRef)
	{
		UE_LOG(LogTemp, Log, TEXT("We do have a character as a owner of the Inventory Component"));
		AStudyPlayerState* PStateRef = Cast<AStudyPlayerState>(CharacterRef->GetPlayerState());
		if(PStateRef)
		{
			UE_LOG(LogTemp, Log, TEXT("We do have a compatible Player State in the Inventory Component"));
			if(PStateRef->GetPawn())
			{
				UE_LOG(LogTemp, Log, TEXT("We do have a Network Pawn using the Player State"));
				AStudyPC* Controller = Cast<AStudyPC>(PStateRef->GetPawn()->GetController());
				if(Controller)
				{
					UE_LOG(LogTemp, Log, TEXT("We do have a compatible Player Controler in the Inventory Component"));
					FVector WLocation;
					FVector WDirection = FVector(0.f);
					Controller->DeprojectMousePositionToWorld(WLocation, WDirection);
					ETraceTypeQuery query = UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility);
					TArray<AActor*> ignores;
					TArray<FHitResult> result;
					if(PStateRef->GetWorld())
					{
						UE_LOG(LogTemp, Log, TEXT("We do have a Valid world on the Player State"));
						UKismetSystemLibrary::SphereTraceMulti(PStateRef->GetWorld(), WLocation,
							(UKismetMathLibrary::Multiply_VectorFloat(WDirection, 10000.f) + WLocation),
							20.f, query, true, ignores, EDrawDebugTrace::None, result,
							true);
						UE_LOG(LogTemp, Log, TEXT("Trace has been performed inside the Inventory Component"));

						// For each based on the hit results of the trace
						for (FHitResult hit: result)
						{
							if(hit.GetActor())
							{
								float distance = PStateRef->GetPawn()->GetDistanceTo(hit.GetActor());
								APickup* pickup = Cast<APickup>(hit.GetActor());
								if(pickup && distance <= 410.f)
								{
									UE_LOG(LogTemp, Log, TEXT("Pickup near by found during trace, aborting other hits"))
									bFoundSomething = true;
									FVector2D ScreenPosition;
									Controller->ProjectWorldLocationToScreen(hit.Location, ScreenPosition, true);
									CreateItemInfoWidget(Controller, pickup, ScreenPosition);
									OnPickupClick.Broadcast(Controller, pickup, ScreenPosition);
									break;
								}
								
							}
						}

						
						if(!bFoundSomething)
						{
							UE_LOG(LogTemp, Log, TEXT("No Pickups near by were found"))
							UUserWidget* ItemWidgetRef = CharacterRef->GetItemWidgetRef();
							if (ItemWidgetRef) ItemWidgetRef->RemoveFromParent();
						}
					}
		
				}
			}
		}
		
	}
}

void UInventoryComponent::CreateItemInfoWidget(AStudyPC* PlayerController, APickup* PickupRef, FVector2D ScreenPosition)
{
	if(PlayerController && PickupRef)
	{
		if(!PickupInfoWidgetRef)
		{
			PickupInfoWidgetRef = CreateWidget<UPickupInfo>(PlayerController, PickupInfoWidgetClass);
		}
		PickupInfoWidgetRef->PickupReference = PickupRef;
		PickupInfoWidgetRef->SetPositionInViewport(ScreenPosition);
		
		if(PickupInfoWidgetRef->IsInViewport())
		{
			PickupInfoWidgetRef->RemoveFromParent();
		} 
		PickupInfoWidgetRef->AddToViewport();
		UE_LOG(LogTemp, Log, TEXT("Pickup Info Widget was created on the viewport"));
	} else
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid Pickup or Controller Passed during pickup info widget creation"));
	}
	
}


void UInventoryComponent::PresentItem3D()
{
	if(GetOwner() && PickupInfoWidgetRef)
	{
		AStudyCharacter* CharacterRef = Cast<AStudyCharacter>(GetOwner());
		if(CharacterRef)
		{
			AStudyPC* ControllerRef = Cast<AStudyPC>(CharacterRef->GetController());
			if(ControllerRef)
			{
				if(!PreviewWidgetRef)
				{
					PreviewWidgetRef = Cast<UItem3DDescription>(CreateWidget<UItem3DDescription>(ControllerRef, Item3DWidgetClass));
				}
				
				if(PickupInfoWidgetRef->PickupReference && PreviewWidgetRef)
				{
					PreviewWidgetRef->ItemDetails = PickupInfoWidgetRef->PickupReference->getItemInfo();
					if(!PreviewWidgetRef->IsInViewport()) PreviewWidgetRef->AddToViewport();
					PickupInfoWidgetRef->RemoveFromParent();
				}
			}
			
		}
		
		
	}
}

void UInventoryComponent::PickupItem(APickup * Item)
{
	AStudyCharacter* CharacterRef = Cast<AStudyCharacter>(GetOwner());
	if(CharacterRef)
	{
		AStudyPlayerState* PSRef = Cast<AStudyPlayerState>(CharacterRef->GetPlayerState());
		if (PSRef)
		{
			Server_PickupItem(Item, PSRef);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Error trying to Cast to Player State"));
		}
	} else
	{
		UE_LOG(LogTemp, Error, TEXT("Error trying to Cast to Player Character even before getting the player State"));
	}
	
}

void UInventoryComponent::Server_PickupItem_Implementation(APickup* Item, AStudyPlayerState* PSRef)
{
	if (GetOwnerRole() == ROLE_Authority || (GetOwnerRole() == ROLE_Authority && GetOwner()->GetRemoteRole() < ROLE_AutonomousProxy))
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
							GetOwner()->FlushNetDormancy();
							UE_LOG(LogTemp, Log, TEXT("Character pickup Item and added to Inventory"));
							Item->Destroy();
							OnPickupItem.Broadcast(Item, PSRef);
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

bool UInventoryComponent::Server_PickupItem_Validate(APickup* Item, AStudyPlayerState* PSRef)
{
	return true;
}

// Drop item on UWorld
void UInventoryComponent::DropItemOnWorld_Implementation(TSubclassOf<AActor> PickupClass, FTransform Location, ESlotType SlotType, int32 SlotID)
{
	UWorld* World = GetWorld();
	AStudyCharacter* CharacterRef = Cast<AStudyCharacter>(GetOwner());
	if ((GetOwnerRole() == ROLE_Authority  || (GetOwnerRole() == ROLE_Authority && GetOwner()->GetRemoteRole() < ROLE_AutonomousProxy)) && PickupClass != NULL && World && CharacterRef)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		World->SpawnActor<AActor>(PickupClass, Location, SpawnParams);
		if (SlotType == ESlotType::ST_ArmorSet && SlotID == 3)
		{
			// Don't change if there's another dual blade on the second hand
			if (CharacterRef->ArmorSetProperties[5].WeaponType.WeaponType != EWeaponType::WT_DualBlade)
			{
				CharacterRef->WeaponBeingUsed = EWeaponType::WT_None;
			}
		}

		if (SlotType == ESlotType::ST_ArmorSet && SlotID == 5)
		{
			// only change if there's no dual blade on the other hand
			if (CharacterRef->ArmorSetProperties[5].WeaponType.WeaponType == EWeaponType::WT_DualBlade && CharacterRef->ArmorSetProperties[3].WeaponType.WeaponType == EWeaponType::WT_None)
			{
				CharacterRef->WeaponBeingUsed = EWeaponType::WT_None;
			}
		}
		UE_LOG(LogTemp, Log, TEXT("The item %s was Dropped"), *UKismetSystemLibrary::GetClassDisplayName(PickupClass));
		OnDropItemOnWorld.Broadcast();
	}
}

bool UInventoryComponent::DropItemOnWorld_Validate(TSubclassOf<AActor> PickupClass, FTransform Location, ESlotType SlotType, int32 SlotID)
{
	return true;
}

void UInventoryComponent::UpdateInventory(AStudyPC* PlayerController, UInventoryDragDropOperation* Operation)
{
	// Update Inventory if dropped from there
	PlayerController->Inventory[Operation->SlotID] = NULL;
	PlayerController->InventoryItems[Operation->SlotID] = FItemDetailsDataTable();
	UE_LOG(LogTemp, Log, TEXT("Dropped item from Inventory"));
	OnUpdateInventory.Broadcast(PlayerController->InventoryItems);
}

void UInventoryComponent::UpdateArmorset(class AStudyCharacter* Character, class UInventoryDragDropOperation* Operation)
{
	Character->ArmorSet[Operation->SlotID] = NULL;
	Character->ArmorSetProperties[Operation->SlotID] = FItemDetailsDataTable();
	UE_LOG(LogTemp, Log, TEXT("Dropped item from Armor Set"));
	OnUpdateArmorSet.Broadcast(Character->ArmorSetProperties);
}


