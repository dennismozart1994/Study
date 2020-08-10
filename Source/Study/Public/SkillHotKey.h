// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CustomVariables.h"
#include "SkillHotKey.generated.h"

/**
 * 
 */
UCLASS()
class STUDY_API USkillHotKey : public UUserWidget
{
	GENERATED_BODY()
    public:
    // Default Layout information
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UButton* SkillButton;
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UImage* Base;
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UImage* SkillIcon;
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UImage* CoolDownImage;
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UTextBlock* CoolDownText;

    // Variables
    UPROPERTY(BlueprintReadOnly, Category="CoolDown")
    class UMaterialInstanceDynamic* CoolDownMaterial;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="CoolDown")
    TSubclassOf<class AMasterSkill> SkillActor;
    UPROPERTY(BlueprintReadWrite, Category="CoolDown")
    class AMasterSkill* SkillRef;
    UPROPERTY(BlueprintReadWrite, Category="Skill Info")
    FSkilDataTable SkillInfo;
    UPROPERTY(BlueprintReadWrite, Category="CoolDown")
    FText CoolDownValueInSeconds;
    UPROPERTY(BlueprintReadOnly, Category="CoolDown")
    bool bCanCastSkill;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill Info")
    int32 SlotIndex;
    UPROPERTY(BlueprintReadOnly, Category = "Skill Info")
    FKey HotKey;
    
    UFUNCTION()
    void OnSlotClicked();
    UFUNCTION()
    void SetCoolDownText(FText content);
    UFUNCTION()
    void SetCoolDownTextVisibility(ESlateVisibility desired);
    UFUNCTION()
    void SetCoolDownImageVisibility(ESlateVisibility desired);
    UFUNCTION()
    void SetSkillIconColour(FLinearColor colour);
    UFUNCTION()
    void SpawnSkill();

    protected:
    virtual bool Initialize() override;
    virtual void NativeConstruct() override;
};
