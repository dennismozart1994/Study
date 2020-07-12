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
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="TimeLine", meta = (AllowPrivateAccess = "true"))
	class UTimelineComponent* CoolDownTimeLine;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Skill Info")
    FSkilDataTable SkillInfo;

	UPROPERTY(BlueprintReadWrite, Category = "Skill Info")
	class USkillHotKey* SkillSlotRef;

	// Declare our delegates that gonna bind the timeline to the update and finish functions
	FOnTimelineEvent UpdateTimelineDelegate{};
	FOnTimelineEvent FinishTimelineDelegate{};

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION()
	void OnTimelineUpdate();

	UFUNCTION()
	void OnTimelineFinished();

	UFUNCTION(BlueprintCallable, Category="CoolDown")
	void CoolDown();

};
