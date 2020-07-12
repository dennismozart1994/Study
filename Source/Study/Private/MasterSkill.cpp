// Fill out your copyright notice in the Description page of Project Settings.


#include "MasterSkill.h"
#include "SkillHotKey.h"
#include "Components/TimelineComponent.h"
#include "Kismet/KismetTextLibrary.h"

// Sets default values
AMasterSkill::AMasterSkill()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CoolDownTimeLine = CreateDefaultSubobject<UTimelineComponent>(TEXT("CoolDownTimeLine"));
	UpdateTimelineDelegate.BindUFunction(this, FName("OnTimelineUpdate"));
	FinishTimelineDelegate.BindUFunction(this, FName("OnTimelineFinished"));
}

// Called when the game starts or when spawned
void AMasterSkill::BeginPlay()
{
	Super::BeginPlay();
	if(CoolDownTimeLine)
	{
		CoolDownTimeLine->SetTimelinePostUpdateFunc(UpdateTimelineDelegate);
        CoolDownTimeLine->SetTimelineFinishedFunc(FinishTimelineDelegate);
    
        if(SkillSlotRef)
        {
        	SkillInfo = SkillSlotRef->SkillInfo;
        	// @TODO substitute this by the Skill Info CoolDown once the equip skill is implemented
        	CoolDownTimeLine->SetTimelineLength(5.f);
        } else	
        {
        	UE_LOG(LogTemp, Error, TEXT("Failed to set the CoolDown time properly, giving it a default value of 5 seconds"));
        	CoolDownTimeLine->SetTimelineLength(5.f);
        }
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load CoolDownTimeline, null pointer found"));
	}
	
	
}

// Called every frame
void AMasterSkill::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMasterSkill::OnTimelineUpdate()
{
	if(SkillSlotRef)
	{
		if(SkillSlotRef->CoolDownMaterial && SkillSlotRef->CoolDownText)
		{
			// Calculated the CoolDown percentage based on it's current position/lenght
			float playbackPosition = CoolDownTimeLine->GetPlaybackPosition();
			float timelineLength = CoolDownTimeLine->GetTimelineLength();
			float coolDownPercentage = 1.f - (playbackPosition/timelineLength);

			// Calculate CoolDown in seconds based on his length - his current value
			const TEnumAsByte<ERoundingMode> RoundingMode = ERoundingMode::HalfToEven;
			FText coolDownInSeconds = UKismetTextLibrary::Conv_FloatToText(
				(CoolDownTimeLine->GetTimelineLength()-CoolDownTimeLine->GetPlaybackPosition()),
				RoundingMode, false,false, 1, 324,
				0, 0);

			// Set the material instance parameter and the text with it's current cool down value in seconds
			SkillSlotRef->CoolDownMaterial->SetScalarParameterValue("Percent", coolDownPercentage);
			SkillSlotRef->SetCoolDownText(coolDownInSeconds);
			UE_LOG(LogTemp, Log, TEXT("Running Cool Down Timeline %s"), *coolDownInSeconds.ToString());
		}
	}
}

void AMasterSkill::OnTimelineFinished()
{
	if(SkillSlotRef)
	{
		SkillSlotRef->SetSkillIconColour(FLinearColor(1.f, 1.f, 1.f, 1.f));
		SkillSlotRef->SetCoolDownImageVisibility(ESlateVisibility::Hidden);
		SkillSlotRef->SetCoolDownTextVisibility(ESlateVisibility::Hidden);
		UE_LOG(LogTemp, Log, TEXT("Finish Cool Down Timeline"));
	}
}

void AMasterSkill::CoolDown()
{
	if(SkillSlotRef)
	{
		SkillSlotRef->SetSkillIconColour(FLinearColor(0.33f, 0.33f, 0.33f, 1.f));
		SkillSlotRef->SetCoolDownImageVisibility(ESlateVisibility::SelfHitTestInvisible);
		SkillSlotRef->SetCoolDownTextVisibility(ESlateVisibility::SelfHitTestInvisible);

		if(CoolDownTimeLine)
		{
			CoolDownTimeLine->PlayFromStart();
		} else
		{
			UE_LOG(LogTemp, Error, TEXT("Unable to grab the CoolDownTimeLine"));
		}
		
	}
}



