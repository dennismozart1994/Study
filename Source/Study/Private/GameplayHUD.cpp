// Fill out your copyright notice in the Description page of Project Settings.

#include "GameplayHUD.h"
#include "CustomVariables.h"
#include "StudyCharacter.h"
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
			StudyCharacterRef->DropItemOnWorld(Operation->ItemClass, LocationToSpawn, Operation->SlotType, Operation->SlotID);

			// Update Inventory if dropped from there
			if (Operation->SlotType == ESlotType::ST_Inventory)
			{
				PCRef->Inventory[Operation->SlotID] = NULL;
				PCRef->InventoryItems[Operation->SlotID] = FItemDetailsDataTable();
				UE_LOG(LogTemp, Log, TEXT("Dropped item from Inventory"));
				UpdateSlots();
			}

			// update armorset if dropped from there
			if (Operation->SlotType == ESlotType::ST_ArmorSet)
			{
				StudyCharacterRef->ArmorSet[Operation->SlotID] = NULL;
				StudyCharacterRef->ArmorSetProperties[Operation->SlotID] = FItemDetailsDataTable();
				UE_LOG(LogTemp, Log, TEXT("Dropped item from Armor Set"));
				UpdateSlots();
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