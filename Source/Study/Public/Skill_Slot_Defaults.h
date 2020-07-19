// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// UserWidget includes like components, helpers, etc
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "CustomVariables.h"
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
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UImage* SkillLocker;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slot Properties")
	int32 SlotIndex;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slot Properties")
	UTexture2D* SlotBackground;

	// Widget Details Class
	UPROPERTY(EditDefaultsOnly, Category = "Skill Details")
    TSubclassOf<class USkillDescription> wSkillDetails;
	UPROPERTY(BlueprintReadOnly, Category = "Skill Details")
    class USkillDescription* SkillDetailsWG;
	
	// Data Table Properties
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data Table")
	class UDataTable* DetailsTable;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data Table", meta = (AllowPrivateAccess = "true"))
	FName SkillRow;
	
	//////////////////////////////////////////////////// FUNCTIONS ////////////////////////////////////////////////////////////
	// Get Custom pointers
	UFUNCTION(BlueprintPure, Category = "References")
	class AStudyPC* GetCustomController();
	UFUNCTION(BlueprintPure, Category = "References")
	class AStudyCharacter* GetCustomCharacter();
	UFUNCTION(BlueprintPure, Category = "References")
	class AStudyPlayerState* GetCustomPlayerState();
	UFUNCTION()
	void OnSlotClicked();
	UFUNCTION(BlueprintPure, Category = "References")
    FSkilDataTable getSkillTreeDetails();
	UFUNCTION(BlueprintPure, Category = "References")
	FSkillDetails getSkillInfo();

	protected:
	UTexture2D* LockerImage;
	FSlateBrush Normal;
	FSlateBrush Hovered;
	FButtonStyle DefaultStyle;
	virtual bool Initialize() override;
	virtual void NativeConstruct() override;

	void PlayPreview();
};
