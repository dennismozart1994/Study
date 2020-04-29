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

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	TArray<FSkilDataTable> CharacterSkills;
	
	UFUNCTION()
	AStudyPlayerState* GetPersonalPlayerState();

	AStudyPC();
	
	protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
