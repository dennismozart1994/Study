// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CustomVariables.h"
#include "Components/TimelineComponent.h"
#include "MasterSkill.generated.h"

UCLASS()
class STUDY_API AMasterSkill : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMasterSkill();
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Components", meta = (AllowPrivateAccess = "true"))
	class USceneComponent* RootSceneComponent;
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category="TimeLine", meta = (AllowPrivateAccess = "true"))
	class UTimelineComponent* CoolDownTimeLine;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Skill Info")
    FSkilDataTable SkillInfo;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Skill Info")
	FSkillDetails SkillDetails;

	UPROPERTY(BlueprintReadWrite, Category = "Skill Info")
	class USkillHotKey* SkillSlotRef;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Skill Info")
	int32 SkillSlotIndex;

	// Declare our delegates that gonna bind the timeline to the update and finish functions
	FOnTimelineEvent UpdateTimelineDelegate{};
	FOnTimelineEvent FinishTimelineDelegate{};

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UFUNCTION()
	virtual void OnTimelineUpdate();

	UFUNCTION()
	virtual void OnTimelineFinished();

	UFUNCTION(BlueprintCallable, Category="CoolDown")
	virtual void CoolDown();

};
