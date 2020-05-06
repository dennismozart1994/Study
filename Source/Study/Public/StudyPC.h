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
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Gameplay")
	TArray<FName> CharacterSkills;

	UFUNCTION()
	AStudyPlayerState* GetPersonalPlayerState();

	UFUNCTION(BlueprintPure, Category = "References")
	FSkilDataTable getSkillDetails(FName rowName);

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Gameplay")
	void Server_UnlockSkill(AStudyPC* Controller, FName Skill);

	UFUNCTION(Client, Reliable)
	void Client_UnlockSkill(AStudyPC* Controller, FName Skill);

	AStudyPC();
	
	protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
