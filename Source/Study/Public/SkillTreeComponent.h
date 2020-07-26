// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CustomVariables.h"
#include "SkillTreeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class STUDY_API USkillTreeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USkillTreeComponent();
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data Table")
    class UDataTable* DetailsTable;
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
    TSubclassOf<class USkillDescription> SkillDetailsClass;
	UPROPERTY(BlueprintReadOnly, Category = "Widgets")
    class USkillDescription* SkillDetailsRef;
	UPROPERTY(BlueprintReadOnly, Category = "Skill")
	bool bIsEquippingSkill;

	UFUNCTION()
	void RemoveAllPreviewWidgets();
	UFUNCTION()
	void CreatePreviewWidget(class USkill_Slot_Defaults* SlotRef);
	UFUNCTION(BlueprintPure, Category = "Skill")
	FSkillDetails GetSkillInfo();
	UFUNCTION(BlueprintPure, Category = "Skill")
	FSkilDataTable GetSkillTreeItem(FName RowName);
	UFUNCTION()
	void UnlockSkill(class AStudyPC* Controller);
	UFUNCTION()
	void SkillBarHighlight();
	UFUNCTION()
	void StopSkillBarHighlight();
	UFUNCTION()
	void StartEquipSkill();
	UFUNCTION()
	void EquipSkill(int32 SlotIndex);
	UFUNCTION()
	void UpdateSkillSlots(ESkillClass Tree);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION()
	void PlayPreview();

	UPROPERTY()
	FSkilDataTable SkillToEquip;
	UPROPERTY()
	FName CurrentSkillRow;
};
