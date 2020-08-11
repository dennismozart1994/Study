// Fill out your copyright notice in the Description page of Project Settings.


#include "MasterSkill.h"
#include "SkillHotKey.h"
#include "StudyCharacter.h"
#include "SkillTreeComponent.h"
#include "Components/TimelineComponent.h"
#include "Kismet/KismetTextLibrary.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AMasterSkill::AMasterSkill()
{
	// replication setup
	SetReplicates(true);
	SetReplicateMovement(true);
	bAlwaysRelevant = true;
	CoolDownTimeLine = CreateDefaultSubobject<UTimelineComponent>(TEXT("CoolDownTimeLine"));
	UpdateTimelineDelegate.BindUFunction(this, FName("OnTimelineUpdate"));
	FinishTimelineDelegate.BindUFunction(this, FName("OnTimelineFinished"));
}

void AMasterSkill::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMasterSkill, SkillDetails);
}

// Called when the game starts or when spawned
void AMasterSkill::BeginPlay()
{
	Super::BeginPlay();
	if(CoolDownTimeLine)
	{
		CoolDownTimeLine->SetTimelinePostUpdateFunc(UpdateTimelineDelegate);
        CoolDownTimeLine->SetTimelineFinishedFunc(FinishTimelineDelegate);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load CoolDownTimeline, null pointer found"));
	}
	
	
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
		}
	}
}

void AMasterSkill::OnTimelineFinished()
{
	AStudyCharacter* PlayerRef = Cast<AStudyCharacter>(GetOwner());
	if(SkillSlotRef)
	{
		if(PlayerRef)
		{
			SkillSlotRef->SetSkillIconColour(FLinearColor(1.f, 1.f, 1.f, 1.f));
			SkillSlotRef->SetCoolDownImageVisibility(ESlateVisibility::Hidden);
			SkillSlotRef->SetCoolDownTextVisibility(ESlateVisibility::Hidden);
			SkillSlotRef->bCanCastSkill = true;
			PlayerRef->GetSkillTreeComponent()->bCanCastSkill = true;
			UE_LOG(LogTemp, Log, TEXT("Finish Cool Down Timeline"));
		} else
		{
			UE_LOG(LogTemp, Error, TEXT("Invalid Player Owner"))
		}
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



