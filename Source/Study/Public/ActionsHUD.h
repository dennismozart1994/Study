// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CustomVariables.h"
#include "ActionsHUD.generated.h"

/**
 * 
 */
UCLASS()
class STUDY_API UActionsHUD : public UUserWidget
{
	GENERATED_BODY()

	public:
	// Internal Variables
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skills|Tab")
    TArray<ESkillClass> TabsAvailable;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skills|Tab")
	ESkillClass CurrentTab;

	// UI Elements
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UBorder* SkillBorder;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* WarriorButton;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UButton* ArcherButton;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UButton* MageButton;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* InventoryButton;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class USkillHotKey* SkillSlot0;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class USkillHotKey* SkillSlot1;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class USkillHotKey* SkillSlot2;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class USkillHotKey* SkillSlot3;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class USkillHotKey* SkillSlot4;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class USkillHotKey* SkillSlot5;
	
	// Blueprint Events
	UFUNCTION(BlueprintImplementableEvent)
	void PlayHighlightAnimation();
	UFUNCTION(BlueprintImplementableEvent)
	void StopHighlightAnimation();

	// Internal Functions
	UFUNCTION()
	void StartAnimation();
	UFUNCTION()
	void CancelAnimation();

	// Interactions with the UI
	UFUNCTION()
	void OnWarriorClicked();
	UFUNCTION()
    void OnArcherClicked();
	UFUNCTION()
    void OnMageClicked();

	// Design Functions
	UFUNCTION()
	void UpdateButtonsVisibility();
	UFUNCTION()
	void SetTabAsActive(ESkillClass Tab);
	UFUNCTION()
	void SetTabAsDeactivated(ESkillClass Tab);

	protected:
    virtual bool Initialize() override;
	virtual void NativeConstruct() override;
	const FSlateColor ActiveColour = FSlateColor(FLinearColor(0.08f, 0.08f, 0.08f, 1.f));
	const FSlateColor NormalColour = FSlateColor(FLinearColor(1.f, 1.f, 1.f, 1.f));
};
