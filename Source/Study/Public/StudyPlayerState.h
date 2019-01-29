// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/PlayerState.h"
#include "CustomVariables.h"
#include "Engine/DataTable.h"
#include "Pickup.h"
#include "StudyPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class STUDY_API AStudyPlayerState : public APlayerState
{
	GENERATED_BODY()
	
	// Variables
	public:
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category="Gameplay")
	FMyStats CharacterStats;

	UPROPERTY(ReplicatedUsing=OnRep_IsAlive, VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay")
	bool bIsAlive;

	// Functions 
	public:
	AStudyPlayerState();

	UFUNCTION()
	virtual void OnRep_IsAlive();

	protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void updateCharStats(UDataTable* DataTable, FName Actual, FName New);
};
