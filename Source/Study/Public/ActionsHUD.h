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
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skills|Tab")
    TArray<ESkillClass> TabsAvailable;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skills|Tab")
	ESkillClass CurrentTab;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* WarriorButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UButton* ArcherButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UButton* MageButton;

	UFUNCTION()
	void OnWarriorClicked();

	UFUNCTION()
    void OnArcherClicked();

	UFUNCTION()
    void OnMageClicked();

	protected:
    virtual bool Initialize() override;
	virtual void NativeConstruct() override;
	const FSlateColor ActiveColour = FSlateColor(FLinearColor(0.08f, 0.08f, 0.08f, 1.f));
	const FSlateColor NormalColour = FSlateColor(FLinearColor(1.f, 1.f, 1.f, 1.f));
	void SetTabAsActive(ESkillClass Tab);
	void SetTabAsDeactivated(ESkillClass Tab);
};
