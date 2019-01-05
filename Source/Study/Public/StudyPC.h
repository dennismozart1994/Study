// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Net/UnrealNetwork.h"
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

	AStudyPC();
	
	protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
