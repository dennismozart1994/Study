// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SkillTreeHUD.generated.h"

/**
 * 
 */
UCLASS()
class USkillTreeHUD : public UUserWidget
{
	GENERATED_BODY()
	
	 protected:
	 UFUNCTION(BlueprintImplementableEvent)
	 void UpdateSlots();
	
	
};
