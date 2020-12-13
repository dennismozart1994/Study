// Fill out your copyright notice in the Description page of Project Settings.


#include "MasterSkill.h"
#include "SkillHotKey.h"
#include "GameplayHUD.h"
#include "StudyCharacter.h"
#include "SkillTreeComponent.h"
#include "Components/SceneComponent.h"
#include "Components/TimelineComponent.h"
#include "Kismet/KismetTextLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
AMasterSkill::AMasterSkill()
{
	// replication setup
	bReplicates = true;
	SetReplicateMovement(true);
	bAlwaysRelevant = true;
	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSkillRoot"));
	RootComponent = RootSceneComponent;
	CoolDownTimeLine = CreateDefaultSubobject<UTimelineComponent>(TEXT("CoolDownTimeLine"));
	UpdateTimelineDelegate.BindUFunction(this, FName("OnTimelineUpdate"));
	FinishTimelineDelegate.BindUFunction(this, FName("OnTimelineFinished"));
	SkillSlotIndex = 0;
}

void AMasterSkill::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMasterSkill, SkillDetails);
	DOREPLIFETIME(AMasterSkill, SkillSlotIndex);
	DOREPLIFETIME(AMasterSkill, CoolDownTimeLine);
}

// Called when the game starts or when spawned
void AMasterSkill::BeginPlay()
{
	Super::BeginPlay();
	if(GetLocalRole() == ROLE_Authority)
	{
		UE_LOG(LogTemp, Log, TEXT("Server is the Skill Owner"))
		if(CoolDownTimeLine)
		{
			CoolDownTimeLine->SetTimelinePostUpdateFunc(UpdateTimelineDelegate);
			CoolDownTimeLine->SetTimelineFinishedFunc(FinishTimelineDelegate);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to load CoolDownTimeline, null pointer found"));
		}
	} else
	{
		UE_LOG(LogTemp, Error, TEXT("Role is not authority for Cool Down Begin Play"))
	}
	UE_LOG(LogTemp, Log, TEXT("Skill Owner: %s"), *UKismetSystemLibrary::GetDisplayName(GetOwner()))
}

void AMasterSkill::OnTimelineUpdate()
{
	AStudyCharacter* PlayerRef = Cast<AStudyCharacter>(GetOwner());
	if(PlayerRef)
	{
		USkillTreeComponent* Component = Cast<USkillTreeComponent>(PlayerRef->FindComponentByClass(USkillTreeComponent::StaticClass()));
		if(Component)
		{
			FText coolDownInSeconds;
			float coolDownPercentage;
			if(GetLocalRole() == ROLE_Authority)
			{
				// Calculated the CoolDown percentage based on it's current position/lenght
				float playbackPosition = CoolDownTimeLine->GetPlaybackPosition();
				float timelineLength = CoolDownTimeLine->GetTimelineLength();
				coolDownPercentage = 1.f - (playbackPosition/timelineLength);

				// Calculate CoolDown in seconds based on his length - his current value
				const TEnumAsByte<ERoundingMode> RoundingMode = ERoundingMode::HalfToEven;
				coolDownInSeconds = UKismetTextLibrary::Conv_FloatToText(
                    (CoolDownTimeLine->GetTimelineLength()-CoolDownTimeLine->GetPlaybackPosition()),
                    RoundingMode, false,false, 1, 324,
                    0, 0);
			} else
			{
				UE_LOG(LogTemp, Error, TEXT("Role is not authority for Update Cool Down Timeline"))
			}
			
			Component->CoolDownUpdate(SkillSlotIndex, coolDownPercentage, coolDownInSeconds);
		} else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to Cast to Component on Master Skill"))
		}
	} else
	{
		UE_LOG(LogTemp, Warning, TEXT("Fail to Cast to Player Character on Master Skill"))
	}
}

void AMasterSkill::OnTimelineFinished()
{
	AStudyCharacter* PlayerRef = Cast<AStudyCharacter>(GetOwner());
	if(PlayerRef)
	{
		USkillTreeComponent* Component = Cast<USkillTreeComponent>(PlayerRef->FindComponentByClass(USkillTreeComponent::StaticClass()));
		if(Component)
		{
			Component->CoolDownFinished(SkillSlotIndex);
			UE_LOG(LogTemp, Log, TEXT("Finish Cool Down Timeline"));
		} else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to Cast to Component on Master Skill"))
		}
	} else
	{
		UE_LOG(LogTemp, Warning, TEXT("Fail to Cast to Player Character on Master Skill"))
	}
}

void AMasterSkill::CoolDown()
{
	AStudyCharacter* PlayerRef = Cast<AStudyCharacter>(GetOwner());
	if(PlayerRef)
	{
		USkillTreeComponent* Component = Cast<USkillTreeComponent>(PlayerRef->FindComponentByClass(USkillTreeComponent::StaticClass()));
		if(Component)
		{
			Component->CoolDown(SkillSlotIndex);
			if(GetLocalRole() == ROLE_Authority)
			{
				if(CoolDownTimeLine)
				{
					CoolDownTimeLine->PlayFromStart();
					UE_LOG(LogTemp, Log, TEXT("Cool Down Timeline start"))
				} else
				{
					UE_LOG(LogTemp, Error, TEXT("Unable to grab the CoolDownTimeLine"));
				}
			} else
			{
				UE_LOG(LogTemp, Error, TEXT("Role is not authority to start Cool Down"))
			}
		} else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to Cast to Component on Master Skill"))
		}
	} else
	{
		UE_LOG(LogTemp, Warning, TEXT("Fail to Cast to Player Character on Master Skill"))
	}
}