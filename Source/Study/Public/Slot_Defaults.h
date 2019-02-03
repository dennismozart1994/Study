// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CustomVariables.h"
#include "Blueprint/UserWidget.h"
#include "Delegates/DelegateCombinations.h"
#include "Slot_Defaults.generated.h"

/**
 * 
 */

UCLASS()
class STUDY_API USlot_Defaults : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data Table")
	class UDataTable* DetailsTable;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slot Properties")
	int32 SlotIndex;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slot Properties")
	UTexture2D* SlotBackground;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slot Properties")
	UTexture2D* DefaultImage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slot Properties")
	ESlotType SlotType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slot Properties")
	EArmorType ArmorSetType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Info")
	FItemDetailsDataTable ItemInfo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item Info")
	TSubclassOf<AActor> Mesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drag and Drop Operation")
	TSubclassOf<UUserWidget> SlotDraggableWG;
	UPROPERTY(BlueprintReadOnly, Category = "Item")
	TSubclassOf<AActor> FromItem;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drag and Drop Operation")
	class UInventoryDragDropOperation* CustomOperation;

protected:
	virtual FReply NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	TArray<TSubclassOf<AActor>> GetInventory();
	TArray<TSubclassOf<AActor>> GetArmorSet();
	FMyStats GetCharacterStats();

	void InventorySwap(UInventoryDragDropOperation* CustomizeOperation);
	void InventoryToArmorSet(UInventoryDragDropOperation* CustomizeOperation);
	void ArmorSetToInventory(UInventoryDragDropOperation* CustomizeOperation);
	void SetFromItem();
};
