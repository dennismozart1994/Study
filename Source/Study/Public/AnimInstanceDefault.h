// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "AnimInstanceDefault.generated.h"

/**
 * 
 */
UCLASS()
class STUDY_API UAnimInstanceDefault : public UAnimInstance
{
	GENERATED_BODY()
	
	protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float CharacterSpeed;

	UFUNCTION(BlueprintCallable, Category = "Animations")
	virtual void UpdateAnimProperties();
};
