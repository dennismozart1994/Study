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
	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere, Category="Gameplay")
	TArray<TSubclassOf<AActor>> Inventory;
	
	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere, Category = "Gameplay")
	TArray<FItemDetailsDataTable> InventoryItems;

	// This will store all indexes to the Unlocked Skill Data table
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Gameplay")
	TArray<FName> CharacterSkills;

	UPROPERTY(BlueprintReadWrite, Category = "Gameplay")
	int32 WarriorTreeCurrentLvl;

	UPROPERTY(BlueprintReadWrite, Category = "Gameplay")
	int32 ArchierTreeCurrentLvl;

	UPROPERTY(BlueprintReadWrite, Category = "Gameplay")
	int32 MageCurrentTreeLvl;

	UFUNCTION()
	AStudyPlayerState* GetPersonalPlayerState();

	AStudyPC();
	
	protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
