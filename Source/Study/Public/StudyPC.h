// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "StudyPC.generated.h"

/**
 * 
 */
UCLASS()
class STUDY_API AStudyPC : public APlayerController
{
	GENERATED_BODY()

	public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Gameplay")
	TArray<TSubclassOf<AActor>> Inventory;

	AStudyPC();
	
};
