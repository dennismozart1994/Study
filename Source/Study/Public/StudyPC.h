// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CustomVariables.h"
#include "StudyPC.generated.h"

/**
 * 
 */

UCLASS()
class STUDY_API AStudyPC : public APlayerController
{
	GENERATED_BODY()

	public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data Table")
	class UDataTable* SkillTable;

	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere, Category="Gameplay")
	TArray<TSubclassOf<AActor>> Inventory;
	
	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere, Category = "Gameplay")
	TArray<FItemDetailsDataTable> InventoryItems;

	// This will store all indexes to the Unlocked Skill Data table
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Skills|Warrior")
    TArray<FName> CharacterSkills;
	
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Skills|Warrior")
	TArray<FName> WarriorSkills;

	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Skills|Archer")
    TArray<FName> ArcherSkills;

	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Skills|Mage")
    TArray<FName> MageSkills;

	UFUNCTION()
	class AStudyPlayerState* GetPersonalPlayerState();

	UFUNCTION(BlueprintPure, Category = "References")
	FSkilDataTable getSkillDetails(FName rowName);

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Gameplay")
	void Server_UnlockSkill(AStudyPC* Controller, FName Skill);

	UFUNCTION(Client, Reliable)
	void Client_UnlockSkill(AStudyPC* Controller, FName Skill);

	UFUNCTION(Client, Reliable, WithValidation, BlueprintCallable, Category = "Gameplay")
    void Server_EquipSkill(AStudyPC* Controller, FName Skill, int32 SlotIndex);

	UFUNCTION(Client, Reliable)
	void Client_EquipSkill(AStudyPC* Controller, FName Skill, int32 SlotIndex);

	UFUNCTION()
	TArray<FName> GetSkillArray(ESkillClass Tree) const;
	
	AStudyPC();
	
	protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
