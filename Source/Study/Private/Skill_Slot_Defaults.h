// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/Actor.h"
#include "CustomVariables.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Skill_Slot_Defaults.generated.h"

/**
 * 
 */
UCLASS()
class USkill_Slot_Defaults : public UUserWidget
{
	GENERATED_BODY()

	public:
	// Default Layout information
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* SkillSlot;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UImage* SkillThumbnail;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slot Properties")
	int32 SlotIndex;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slot Properties")
	UTexture2D* SlotBackground;

	// Data Table Properties
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data Table")
	class UDataTable* DetailsTable;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data Table", meta = (AllowPrivateAccess = "true"))
	FName SkillRow;
	
	//////////////////////////////////////////////////// FUNCTIONS ////////////////////////////////////////////////////////////
	// Get Custom pointers
	class AStudyPC* GetCustomController();
	class AStudyCharacter* GetCustomCharacter();
	class AStudyPlayerState* GetCustomPlayerState();
	UFUNCTION()
	void OnSlotClicked();
	UFUNCTION(BlueprintPure, Category = "Get Item Details")
	FSkilDataTable getSkillDetails();

	protected:
	virtual bool Initialize();
	virtual void NativeConstruct();
};
