// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CustomVariables.h"
#include "Blueprint/UserWidget.h"
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SlotIndex;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* SlotBackground;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* DefaultImage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ESlotType SlotType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EArmorType ArmorSetType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FItemDetails ItemDetails;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item Info")
	TSubclassOf<AActor> Mesh;
};
