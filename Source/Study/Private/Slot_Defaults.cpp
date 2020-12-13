// Fill out your copyright notice in the Description page of Project Settings.

#include "Slot_Defaults.h"

// Custom classes
#include "StudyPC.h"
#include "StudyCharacter.h"
#include "StudyPlayerState.h"
#include "Pickup.h"
#include "InventoryDragDropOperation.h"
#include "Item3DDescription.h"
#include "InventoryComponent.h"

// UserWidget includes like components, helpers, etc
#include "UMG/Public/Blueprint/WidgetBlueprintLibrary.h"
#include "Runtime/UMG/Public/Blueprint/WidgetLayoutLibrary.h"
#include "UMG/Public/Blueprint/WidgetTree.h"

// Math and debugging includes
#include "Engine/Classes/Kismet/KismetSystemLibrary.h"
#include "Engine/Classes/Kismet/KismetMathLibrary.h"
#include "Engine/Engine.h"

bool USlot_Defaults::Initialize()
{
	bool success = Super::Initialize();
	if(!success) return false;

	if(!ensure(Slot_btn != nullptr)) return false;
	Slot_btn->OnClicked.AddDynamic(this, &USlot_Defaults::OnSlotClicked);
	Slot_btn->OnHovered.AddDynamic(this, &USlot_Defaults::OnSlotHovered);
	Slot_btn->OnUnhovered.AddDynamic(this, &USlot_Defaults::OnSlotUnHovered);
	return success;
}

void USlot_Defaults::SetDefaultStyle(UButton* Button)
{
	FSlateBrush Normal;
	Normal.DrawAs = ESlateBrushDrawType::Image;
	Normal.Tiling = ESlateBrushTileType::NoTile;
	Normal.Mirroring = ESlateBrushMirrorType::NoMirror;
	Normal.ImageSize = FVector2D(32.f, 32.f);
	Normal.SetResourceObject(SlotBackground);
	Normal.TintColor = FSlateColor(FLinearColor(1.f, 1.f, 1.f, 1.f));
	

	FSlateBrush Hovered;
	Hovered.DrawAs = ESlateBrushDrawType::Image;
	Hovered.Tiling = ESlateBrushTileType::NoTile;
	Hovered.Mirroring = ESlateBrushMirrorType::NoMirror;
	Hovered.ImageSize = FVector2D(32.f, 32.f);
	Hovered.SetResourceObject(SlotBackground);
	Hovered.TintColor = FSlateColor(FLinearColor(0.447917f, 0.447917f, 0.447917f, 1.f));

	FButtonStyle DefaultStyle;

	DefaultStyle.Normal = Normal;
	DefaultStyle.Hovered = Hovered;
	DefaultStyle.Pressed = Normal;

	Button->SetStyle(DefaultStyle);
}

//////////////////////////////////////////////// Native Events //////////////////////////////////////////////
void USlot_Defaults::NativeConstruct()
{
	Super::NativeConstruct();
	TArray<UWidget*> ChildWidgets;
	// Bind delegates here
	if (Slot_btn)
	{
		SetDefaultStyle(Slot_btn);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No Slot Button Found"));
	}
	
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
		if (ItemInfo.Thumbnail != NULL)
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
					CustomOperation->Pivot = EDragPivot::CenterCenter;
					CustomOperation->Payload = this;

					OutOperation = CustomOperation;
					UE_LOG(LogTemp, Log, TEXT("Called Custom On Drag Detected Operation"));
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("Not a valid From class item"));
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Error trying to create Draggable Widget"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("There are no items to start dragging"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Called Default On Drag Detected"));
	}
}

// Dealing Drop Operations
bool USlot_Defaults::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	UInventoryDragDropOperation* Operation = Cast<UInventoryDragDropOperation>(InOperation);
	if (Operation)
	{
		// Inventory Swap on Drop
		if ((Operation->SlotType == ESlotType::ST_Inventory) && (SlotType == ESlotType::ST_Inventory))
		{
			UE_LOG(LogTemp, Log, TEXT("Inventory Swap on Drop Called"));
			InventorySwap(Operation);
		}

		// Send from Inventory to ArmorSet
		if ((Operation->SlotType == ESlotType::ST_Inventory) && (SlotType == ESlotType::ST_ArmorSet))
		{
			UE_LOG(LogTemp, Log, TEXT("Add ArmorSet Called"));
			InventoryToArmorSet(Operation);
		}

		// Remove ArmorSet back to Inventory
		if ((Operation->SlotType == ESlotType::ST_ArmorSet) && (SlotType == ESlotType::ST_Inventory))
		{
			UE_LOG(LogTemp, Log, TEXT("Remove ArmorSet Called"));
			ArmorSetToInventory(Operation);
		}
		UpdateSlots();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Couldn't Cast to Custom operation"));
	}
	return true;
}

void USlot_Defaults::NativeOnDragEnter(const FGeometry & InGeometry, const FDragDropEvent & InDragDropEvent, UDragDropOperation * InOperation)
{
	Super::NativeOnDragEnter(InGeometry, InDragDropEvent, InOperation);
	if (Slot_btn)
	{
		Slot_btn->SetStyle(DragStyle);
	}
}

void USlot_Defaults::NativeOnDragLeave(const FDragDropEvent & InDragDropEvent, UDragDropOperation * InOperation)
{
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);
	if (Slot_btn)
	{
		SetDefaultStyle(Slot_btn);
	}
}

///////////////////////////////////////////// Components Delegates ///////////////////////////////////////////
void USlot_Defaults::OnSlotClicked()
{
	UE_LOG(LogTemp, Log, TEXT("On Clicked Called"));
	if (SlotType == ESlotType::ST_ArmorSet || SlotType == ESlotType::ST_Inventory)
	{
		if(this->GetOwningPlayerPawn())
		{
			UInventoryComponent* Component = Cast<UInventoryComponent>(
                this->GetOwningPlayerPawn()->FindComponentByClass(UInventoryComponent::StaticClass()));
			if(Component) Component->PresentItem3D();
			Call3DPreview();
		}
	}
}

void USlot_Defaults::OnSlotHovered()
{
	if (SlotType == ESlotType::ST_Inventory && ItemInfo.Thumbnail != NULL)
	{
		// Create Item Info Widget on BP
		if (ItemInfo.ItemType == EItemType::IT_ArmorSet)
		{
			CreateComparisionUI(ESlateVisibility::Visible);
		}
		else
		{
			CreateComparisionUI(ESlateVisibility::Hidden);
		}
		
		// After Creating add to ViewPort on the Position Calculated (Mouse + Offset)
		UWidgetLayoutLibrary* WidgetLayoutLibrary;
		WidgetLayoutLibrary = NewObject<UWidgetLayoutLibrary>(UWidgetLayoutLibrary::StaticClass());
		if (WidgetLayoutLibrary)
		{
			FVector2D ViewportPosition = WidgetLayoutLibrary->GetMousePositionOnViewport(GetWorld());
			if (UIComparisionRef)
			{
				if(!UIComparisionRef->IsInViewport()) UIComparisionRef->AddToViewport(0);
				float Y;
				float X;
				UKismetMathLibrary::BreakVector2D(ViewportPosition, X, Y);
				X += 70.f;
				Y += -70.f;
				FVector2D Position = UKismetMathLibrary::MakeVector2D(X, Y);

				UIComparisionRef->SetPositionInViewport(Position, false);
			}
		}
	}
}

void USlot_Defaults::OnSlotUnHovered()
{
	// Remove From Parent the Item Comparision UI when Unhovered
	if (UIComparisionRef)
	{
		UIComparisionRef->RemoveFromParent();
	}
}

//////////////////////////////////////////////////// FUNCTIONS ////////////////////////////////////////////////////////////////////////////////////////////////////////////////// FUNCTIONS //////////////////////////////////////////////////////////////

AStudyPC* USlot_Defaults::GetCustomController()
{
	return Cast<AStudyPC>(GetOwningPlayer());
}

AStudyCharacter* USlot_Defaults::GetCustomCharacter()
{
	return Cast<AStudyCharacter>(GetOwningPlayerPawn());
}

AStudyPlayerState* USlot_Defaults::GetCustomPlayerState()
{
	return Cast<AStudyPlayerState>(GetOwningPlayer()->PlayerState);
}

// Deal Inventory Swap between slots
void USlot_Defaults::InventorySwap(UInventoryDragDropOperation* CustomizeOperation)
{
	AStudyPC* PCRef = GetCustomController();

	if (CustomizeOperation && PCRef)
	{
		// From Slot Variables
		int32 FromSlotIndex = CustomizeOperation->SlotID;
		TSubclassOf<AActor> FromActorClass = CustomizeOperation->ItemClass;
		FItemDetailsDataTable FromItemInfo = CustomizeOperation->ItemDetails;
			
		// To Slot Variables
		int32 ToSlotIndex = SlotIndex;
		TSubclassOf<AActor> ToActorClass = PCRef->Inventory[SlotIndex];
		FItemDetailsDataTable ToItemInfo = ItemInfo;

		UE_LOG(LogTemp, Log, TEXT("Current From item is %s"), *UKismetSystemLibrary::GetDisplayName(FromActorClass));

		// if moving to already filled widget
		if (UKismetSystemLibrary::IsValid(ToItemInfo.Mesh))
		{
			PCRef->Inventory[FromSlotIndex] = PCRef->Inventory[SlotIndex];
			PCRef->InventoryItems[FromSlotIndex] = PCRef->InventoryItems[SlotIndex];

			PCRef->Inventory[ToSlotIndex] = FromActorClass;
			PCRef->InventoryItems[ToSlotIndex] = FromItemInfo;

			UE_LOG(LogTemp, Log, TEXT("Both items are valid"));
		}
		else
		{
			PCRef->Inventory[ToSlotIndex] = FromActorClass;
			PCRef->InventoryItems[ToSlotIndex] = FromItemInfo;

			PCRef->Inventory[FromSlotIndex] = NULL;
			PCRef->InventoryItems[FromSlotIndex] = FItemDetailsDataTable();

			UE_LOG(LogTemp, Log, TEXT("Only the sender item is valid"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Either the DragDropOperation or the Player Controller is null"));
	}
}

// Deal add ArmorSet Item
void USlot_Defaults::InventoryToArmorSet(UInventoryDragDropOperation* CustomizeOperation)
{
	AStudyPC* PCRef = GetCustomController();
	AStudyCharacter* CharRef = GetCustomCharacter();
	AStudyPlayerState* PSRef = GetCustomPlayerState();

	if (CustomizeOperation && PCRef && CharRef && PSRef)
	{
		if (CustomizeOperation->ItemDetails.ArmorType == ArmorSetType)
		{
			if (PSRef->CharacterStats.CurrentLevel >= ItemInfo.GoldLevelRequired)
			{
				// From Item Data
				int32 FromSlotIndex = CustomizeOperation->SlotID;
				TSubclassOf<AActor> FromItemClass = CustomizeOperation->ItemClass;
				FItemDetailsDataTable FromItemInfo = CustomizeOperation->ItemDetails;

				// To Item Data
				int32 ToSlotIndex = SlotIndex;
				TSubclassOf<AActor> ToItemClass = CharRef->ArmorSet[ToSlotIndex];
				FItemDetailsDataTable ToItemInfo = ItemInfo;

				// Swap Armorset
				if (UKismetSystemLibrary::IsValid(ToItemInfo.Mesh))
				{
					PCRef->Inventory[FromSlotIndex] = ToItemClass;
					PCRef->InventoryItems[FromSlotIndex] = ToItemInfo;

					CharRef->ArmorSet[ToSlotIndex] = FromItemClass;
					CharRef->ArmorSetProperties[ToSlotIndex] = FromItemInfo;

					// Call Update Stats Custom Event Inside Main Character
					PSRef->Server_updateCharacterStats(ToItemInfo, FromItemInfo);
					UE_LOG(LogTemp, Log, TEXT("Called Server Update Stats"));
				}
				else
				{
					PCRef->Inventory[FromSlotIndex] = NULL;
					PCRef->InventoryItems[FromSlotIndex] = FItemDetailsDataTable();

					CharRef->ArmorSet[ToSlotIndex] = FromItemClass;
					CharRef->ArmorSetProperties[ToSlotIndex] = FromItemInfo;

					// Call Update Stats Custom Event Inside Main Character
					PSRef->Server_updateCharacterStats(ToItemInfo, FromItemInfo);
					UE_LOG(LogTemp, Log, TEXT("Called Server Update Stats"));
				}
				UE_LOG(LogTemp, Log, TEXT("Add Item to ArmorSet"));
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
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("One of the main Ref is null"));
	}
}

// Remove ArmorSet back to Inventory
void USlot_Defaults::ArmorSetToInventory(UInventoryDragDropOperation* CustomizeOperation)
{
	AStudyPC* PCRef = GetCustomController();
	AStudyCharacter* CharRef = GetCustomCharacter();
	AStudyPlayerState* PSRef = GetCustomPlayerState();

	if (CustomizeOperation && PCRef && CharRef && PSRef )
	{
		// From Item Data
		int32 FromSlotIndex = CustomizeOperation->SlotID;
		TSubclassOf<AActor> FromItemClass = CustomizeOperation->ItemClass;
		FItemDetailsDataTable FromItemInfo = CustomizeOperation->ItemDetails;

		// To Item Data
		int32 ToSlotIndex = SlotIndex;
		TSubclassOf<AActor> ToItemClass = PCRef->Inventory[ToSlotIndex];
		FItemDetailsDataTable ToItemInfo = ItemInfo;

		// Remove item from ArmorSet to Empty Inventory Slot
		if (!UKismetSystemLibrary::IsValid(ToItemInfo.Mesh))
		{
			// Remove Item from Armor Set
			PSRef->Server_updateCharacterStats(FromItemInfo, FItemDetailsDataTable());
			UE_LOG(LogTemp, Log, TEXT("Removed from Armorset"));
			
			// Do the actual Swap
			PCRef->Inventory[ToSlotIndex] = FromItemClass;
			PCRef->InventoryItems[ToSlotIndex] = FromItemInfo;

			CharRef->ArmorSet[FromSlotIndex] = NULL;
			CharRef->ArmorSetProperties[FromSlotIndex] = FItemDetailsDataTable();
		}
		// swap items on Armor set if they are the same armor type
		else if (PCRef->InventoryItems[ToSlotIndex].ItemType == EItemType::IT_ArmorSet && CharRef->ArmorSetProperties[FromSlotIndex].ArmorType == PCRef->InventoryItems[ToSlotIndex].ArmorType)
		{
			PSRef->Server_updateCharacterStats(FromItemInfo, ToItemInfo);
			UE_LOG(LogTemp, Log, TEXT("Swap between Armorsets"));

			CharRef->ArmorSet[FromSlotIndex] = ToItemClass;
			CharRef->ArmorSetProperties[FromSlotIndex] = ToItemInfo;

			PCRef->Inventory[ToSlotIndex] = FromItemClass;
			PCRef->InventoryItems[ToSlotIndex] = FromItemInfo;
		}
		// Remove ArmorSet to an already assigned inventory slot of a different ArmorType (Try to find another slot to remove the armorSet)
		else
		{
			bool bCanSendToInventory = false;
			for (int32 i = 0; i < PCRef->Inventory.Num(); i++)
			{
				// only return to inventory if there's enough space
				if (!UKismetSystemLibrary::IsValid(PCRef->InventoryItems[i].Mesh))
				{
					bCanSendToInventory = true;
					CharRef->ArmorSet[FromSlotIndex] = ToItemClass;
					CharRef->ArmorSetProperties[FromSlotIndex] = ToItemInfo;
					
					// Remove from ArmorSet
					PSRef->Server_updateCharacterStats(FromItemInfo, FItemDetailsDataTable());
					UE_LOG(LogTemp, Log, TEXT("Removed from Armorset"));
					break;
				}
			}
			if (bCanSendToInventory)
			{
				UE_LOG(LogTemp, Log, TEXT("Moved Successfully"));
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Your Inventory Is Full, so you can't move this item the item to the Inventory"));
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("One of the bew references are null"));
	}
}

void USlot_Defaults::SetFromItem()
{
	AStudyPC* PCRef = GetCustomController();
	AStudyCharacter* CharRef = GetCustomCharacter();

	if (PCRef && CharRef)
	{
		if (SlotType == ESlotType::ST_Inventory && UKismetSystemLibrary::IsValidClass(PCRef->Inventory[SlotIndex]))
		{
			FromItem = PCRef->Inventory[SlotIndex];
		}

		if (SlotType == ESlotType::ST_ArmorSet && UKismetSystemLibrary::IsValidClass(CharRef->ArmorSet[SlotIndex]))
		{
			FromItem = CharRef->ArmorSet[SlotIndex];
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Null pointers on SETFROMITEM() function"));
	}
}
