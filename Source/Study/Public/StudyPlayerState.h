// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/PlayerState.h"
#include "CustomVariables.h"
#include "StudyPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class STUDY_API AStudyPlayerState : public APlayerState
{
	GENERATED_BODY()
	
	public:
	AStudyPlayerState();

	protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	protected:
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category="Gameplay")
	FMyStats CharacterStats;
};
