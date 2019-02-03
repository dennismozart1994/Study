// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "CustomVariables.h"
#include "InventoryDragDropOperation.generated.h"

/**
 * 
 */
UCLASS()
class STUDY_API UInventoryDragDropOperation : public UDragDropOperation
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 SlotID;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	ESlotType SlotType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AActor> ItemClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FItemDetailsDataTable ItemDetails;
};
