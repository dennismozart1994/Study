// Fill out your copyright notice in the Description page of Project Settings.

#include "GameplayHUD.h"
#include "ActionsHUD.h"
#include "CustomVariables.h"
#include "StudyCharacter.h"
#include "InventoryComponent.h"
#include "StudyPC.h"
#include "InventoryDragDropOperation.h"
#include "Kismet/KismetMathLibrary.h"

// Dealing Drop Operations
bool UGameplayHUD::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	UInventoryDragDropOperation* Operation = Cast<UInventoryDragDropOperation>(InOperation);
	if (Operation)
	{
		AStudyCharacter* StudyCharacterRef = Cast<AStudyCharacter>(GetOwningPlayer()->GetPawn());
		AStudyPC* PCRef = Cast<AStudyPC>(GetOwningPlayer());
		if (StudyCharacterRef && PCRef)
		{
			FVector Location = StudyCharacterRef->GetActorLocation() + StudyCharacterRef->GetActorForwardVector() * 100;
			FRotator Rotation = StudyCharacterRef->GetActorRotation();
			FTransform LocationToSpawn = UKismetMathLibrary::MakeTransform(Location, Rotation, FVector(1.f));
			// Spawn Item dropped on World
			UInventoryComponent* Component = Cast<UInventoryComponent>(
            this->GetOwningPlayerPawn()->FindComponentByClass(UInventoryComponent::StaticClass()));
			if(Component)
			{
				Component->DropItemOnWorld(Operation->ItemClass, LocationToSpawn, Operation->SlotType, Operation->SlotID);
				// Update Inventory if dropped from there
				if (Operation->SlotType == ESlotType::ST_Inventory)
				{
					Component->UpdateInventory(PCRef, Operation);
					UpdateSlots();
				}

				// update armorset if dropped from there
				if (Operation->SlotType == ESlotType::ST_ArmorSet)
				{
					Component->UpdateArmorset(StudyCharacterRef, Operation);
					UpdateSlots();
				}
			} else
			{
				UE_LOG(LogTemp, Error, TEXT("Player Doesn't have The Inventory Component Implemented"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Error Trying to get Character Reference on UI Drop"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Error Trying to get the Operation to handle UI Drop"));
	}
	return true;
}