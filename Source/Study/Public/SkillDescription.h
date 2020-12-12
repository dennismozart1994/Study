// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "CustomVariables.h"
#include "SkillDescription.generated.h"

/**
 * 
 */
UCLASS()
class STUDY_API USkillDescription : public UUserWidget
{
	GENERATED_BODY()
	
	public:
	// Default layout information
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* UnlockButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* UnlockButtonText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* PriceText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* SkillDescription;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* SkillRequirements;

	// Skill Properties
	UPROPERTY(BlueprintReadOnly, Category = "Skill Info", meta = (AllowPrivateAccess = "true"))
    class USkill_Slot_Defaults* SkillSlotRef;
	
	UPROPERTY(BlueprintReadOnly, Category = "Skill Info", meta = (AllowPrivateAccess = "true"))
	FSkillDetails SkillInfo;

	UPROPERTY(BlueprintReadOnly, Category = "Tree Info", meta = (AllowPrivateAccess = "true"))
	FSkilDataTable SkillTreeInfo;

	// Conditioners to slot behaviour
	bool bCanEquip;
	bool bHasTheProperLvl;
	bool bHasTheProperGold;
	bool bHasUnlockedRequiredSkill;

	// Custom Getters
	UFUNCTION(BlueprintPure, Category = "References")
    class AStudyPC* GetCustomController();
	UFUNCTION(BlueprintPure, Category = "References")
    class AStudyCharacter* GetCustomCharacter();
	UFUNCTION(BlueprintPure, Category = "References")
    class AStudyPlayerState* GetCustomPlayerState();
	UFUNCTION(BlueprintPure, Category = "References")
    FSkilDataTable getSkillTreeDetails(FName SkillRow);
	UFUNCTION(BlueprintPure, Category = "References")
    FSkillDetails getSkillInfo(class TSubclassOf<AMasterSkill> SkillClass);

	// UI Bindings
	UFUNCTION()
	void OnUnlockClicked();

	protected:
	virtual bool Initialize();
	virtual void NativeConstruct();

	private:
	const FName COMMON_STRING_TABLE = ACustomVariables::COMMON_WORDS;
};
