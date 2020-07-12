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
    UPROPERTY(EditDefaultsOnly, Category="CoolDown")
    TSubclassOf<class AActor> CoolDownActor;
    UPROPERTY(BlueprintReadWrite, Category="CoolDown")
    class AActor* CoolDownRef;
    UPROPERTY(BlueprintReadWrite, Category="Skill Info")
    FSkilDataTable SkillInfo;
    
    UFUNCTION()
    void OnSlotClicked();
    UFUNCTION(BlueprintImplementableEvent)
    void CoolDownTimeline();

    protected:
    virtual bool Initialize() override;
    virtual void NativeConstruct() override;
};
