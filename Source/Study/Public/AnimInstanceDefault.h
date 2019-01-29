// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "StudyCharacter.h"
#include "StudyPlayerState.h"
#include "AnimInstanceDefault.generated.h"

/**
 * 
 */
UCLASS()
class STUDY_API UAnimInstanceDefault : public UAnimInstance
{
	GENERATED_BODY()
	public:
	UAnimInstanceDefault();

	protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animations Properties")
	float CharacterSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animations Properties")
	bool bIsAlive;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Functions")
	bool bDoOnce;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animations Properties")
	int32 DeathIndex;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "References")
	AStudyCharacter* CharacterRef;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "References")
	AStudyPlayerState* PlayerStateRef;

	UFUNCTION(BlueprintCallable, Category = "Animations")
	virtual void UpdateAnimProperties();

	UFUNCTION()
	void DoOnce();

	UFUNCTION()
	void ResetDoOnce();
};
