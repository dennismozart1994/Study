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

	UFUNCTION()
	void RemoveAllPreviewWidgets();

	UFUNCTION()
	void CreatePreviewWidget(class USkill_Slot_Defaults* SlotRef);

	UFUNCTION(BlueprintPure, Category = "Skill")
	FSkillDetails GetSkillInfo();

	UFUNCTION(BlueprintPure, Category = "Skill")
	FSkilDataTable GetSkillTreeItem(FName RowName);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION()
	void PlayPreview();
};
