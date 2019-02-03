// Fill out your copyright notice in the Description page of Project Settings.

#include "Slot_Defaults.h"
#include "Engine/Classes/Kismet/KismetSystemLibrary.h"
#include "Engine/Classes/Kismet/KismetMathLibrary.h"
#include "StudyPC.h"
#include "StudyCharacter.h"
#include "StudyPlayerState.h"
#include "Pickup.h"
#include "Engine/DataTable.h"
#include "Engine/Classes/Engine/World.h"
#include "UMG/Public/Blueprint/WidgetBlueprintLibrary.h"
#include "InventoryDragDropOperation.h"

void USlot_Defaults::NativeConstruct()
{
	Super::NativeConstruct();

	// Bind delegates here
}

// Condition to Start Dragging
FReply USlot_Defaults::NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnPreviewMouseButtonDown(InGeometry, InMouseEvent);
	UE_LOG(LogTemp, Log, TEXT("Called on Mouse Key button Down"));
	FEventReply reply = UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton);
	return reply.NativeReply;
}

// Deal Drag Operation and Visuals
void USlot_Defaults::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
	if (DetailsTable)
	{
		SetFromItem();
		USlot_Defaults* Draggable = CreateWidget<USlot_Defaults, APlayerController>(GetOwningPlayer(), SlotDraggableWG, NAME_None);
		if (Draggable)
		{
			Draggable->SlotBackground = SlotBackground;
			Draggable->ItemInfo = ItemInfo;
			CustomOperation = Cast<UInventoryDragDropOperation>(UWidgetBlueprintLibrary::CreateDragDropOperation(UInventoryDragDropOperation::StaticClass()));
			
			if (CustomOperation && DetailsTable && UKismetSystemLibrary::IsValidClass(FromItem))
			{
				CustomOperation->SlotID = SlotIndex;
				CustomOperation->SlotType = SlotType;
				CustomOperation->ItemClass = FromItem;
				static const FString ContextCurrent(TEXT("Current Item Details"));
				CustomOperation->ItemDetails = ItemInfo;
				CustomOperation->DefaultDragVisual = Draggable;
				CustomOperation->Pivot = EDragPivot::MouseDown;
				CustomOperation->Payload = this;
				
				OutOperation = CustomOperation;
				UE_LOG(LogTemp, Log, TEXT("Called Custom On Drag Detected Operation"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Error trying to create Draggable Widget"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Called Default On Drag Detected"));
		Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
	}
}

// Dealing Drop Operations
bool USlot_Defaults::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	UInventoryDragDropOperation* Operation = Cast<UInventoryDragDropOperation>(InOperation);
	if (Operation)
	{
		// Inventory Swap on Drop
		if ((Operation->SlotType == ESlotType::ST_Inventory) && (SlotType == ESlotType::ST_Inventory))
		{
			InventorySwap(Operation);
			UE_LOG(LogTemp, Log, TEXT("Inventory Swap on Drop Called"));
		}

		// Send from Inventory to ArmorSet
		if ((Operation->SlotType == ESlotType::ST_Inventory) && (SlotType == ESlotType::ST_ArmorSet))
		{
			InventoryToArmorSet(Operation);
			UE_LOG(LogTemp, Log, TEXT("Add ArmorSet Called"));
		}

		// Remove ArmorSet back to Inventory
		if ((Operation->SlotType == ESlotType::ST_ArmorSet) && (SlotType == ESlotType::ST_Inventory))
		{
			ArmorSetToInventory(Operation);
			UE_LOG(LogTemp, Log, TEXT("Remove ArmorSet Called"));
		}
	}
	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}

TArray<TSubclassOf<AActor>> USlot_Defaults::GetInventory()
{
	AStudyPC* PlayerControllerRef = Cast<AStudyPC>(GetOwningPlayer());
	if (PlayerControllerRef)
	{
		return PlayerControllerRef->Inventory;
	}
	else
	{
		TArray<TSubclassOf<AActor>> test;
		return test;
	}
}

TArray<TSubclassOf<AActor>> USlot_Defaults::GetArmorSet()
{
	AStudyCharacter* CharacterRef = Cast<AStudyCharacter>(GetOwningPlayerPawn());
	if (CharacterRef)
	{
		return CharacterRef->ArmorSet;
	}
	else
	{
		TArray<TSubclassOf<AActor>> test;
		return test;
	}
}

FMyStats USlot_Defaults::GetCharacterStats()
{
	AStudyPlayerState* PSRef = Cast<AStudyPlayerState>(GetOwningPlayer()->PlayerState);
	FMyStats MyPlayerStats;

	if (PSRef)
	{
		MyPlayerStats = PSRef->CharacterStats;
	}
	return MyPlayerStats;

}

// Deal Inventory Swap between slots
void USlot_Defaults::InventorySwap(UInventoryDragDropOperation* CustomizeOperation)
{
	if (CustomizeOperation)
	{
		// From Slot Variables
		int32 FromSlotIndex = CustomizeOperation->SlotID;
		TSubclassOf<AActor> FromActorClass = CustomizeOperation->ItemClass;
		FItemDetailsDataTable FromItemInfo = CustomizeOperation->ItemDetails;
			
		// To Slot Variables
		int32 ToSlotIndex = SlotIndex;
		TSubclassOf<AActor> ToActorClass = GetInventory()[SlotIndex];
		FItemDetailsDataTable ToItemInfo = ItemInfo;

		// if moving to already filled widget
		if (UKismetSystemLibrary::IsValidClass(ToActorClass))
		{
			GetInventory()[FromSlotIndex] = GetInventory()[SlotIndex];
			GetInventory()[ToSlotIndex] = FromActorClass;
			// @TODO Assign ItemStruct Inventory Later
		}
		else
		{
			GetInventory()[ToSlotIndex] = FromItem;
			GetInventory()[FromSlotIndex] = NULL;
			// @TODO Assign ItemStruct Inventory Later
		}
	}
}

// Deal add ArmorSet Item
void USlot_Defaults::InventoryToArmorSet(UInventoryDragDropOperation* CustomizeOperation)
{
	if (CustomizeOperation)
	{
		if (ItemInfo.ArmorType == ArmorSetType)
		{
			if (GetCharacterStats().CurrentLevel <= ItemInfo.GoldLevelRequired)
			{
				// From Item Data
				int32 FromSlotIndex = CustomizeOperation->SlotID;
				TSubclassOf<AActor> FromItemClass = CustomizeOperation->ItemClass;
				FItemDetailsDataTable FromItemInfo = CustomizeOperation->ItemDetails;

				// To Item Data
				int32 ToSlotIndex = SlotIndex;
				TSubclassOf<AActor> ToItemClass = GetArmorSet()[ToSlotIndex];
				FItemDetailsDataTable ToItemInfo = ItemInfo;

				if (UKismetSystemLibrary::IsValidClass(ToItemClass))
				{
					
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Does not have the proper level to equip"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Not the proper slot"));
		}
	}
}

void USlot_Defaults::ArmorSetToInventory(UInventoryDragDropOperation* CustomizeOperation)
{

}

void USlot_Defaults::SetFromItem()
{
	TArray<TSubclassOf<AActor>> Inventory = GetInventory();
	TArray<TSubclassOf<AActor>> ArmorSet = GetArmorSet();

	if (SlotType == ESlotType::ST_Inventory && UKismetSystemLibrary::IsValidClass(Inventory[SlotIndex]))
	{
		FromItem = Inventory[SlotIndex];
	}

	if (SlotType == ESlotType::ST_ArmorSet && UKismetSystemLibrary::IsValidClass(ArmorSet[SlotIndex]))
	{
		FromItem = ArmorSet[SlotIndex];
	}
}
