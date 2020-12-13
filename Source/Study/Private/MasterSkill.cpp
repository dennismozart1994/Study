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
	coolDownPercentage = 0.f;
}

void AMasterSkill::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMasterSkill, SkillDetails);
	DOREPLIFETIME(AMasterSkill, SkillSlotIndex);
	DOREPLIFETIME(AMasterSkill, CoolDownTimeLine);
	DOREPLIFETIME(AMasterSkill, coolDownPercentage);
	DOREPLIFETIME(AMasterSkill, coolDownInSeconds);
}

// Called when the game starts or when spawned
void AMasterSkill::BeginPlay()
{
	Super::BeginPlay();
	Server_SetupCoolDown();
	UE_LOG(LogTemp, Log, TEXT("Skill Owner: %s"), *UKismetSystemLibrary::GetDisplayName(GetOwner()))
}

void AMasterSkill::Server_SetupCoolDown_Implementation()
{
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
		Client_SetupCoolDown();
	}
}

bool AMasterSkill::Server_SetupCoolDown_Validate()
{
	return true;
}

void AMasterSkill::Client_SetupCoolDown_Implementation()
{
	Server_SetupCoolDown();
}


void AMasterSkill::OnTimelineUpdate()
{
	AStudyCharacter* PlayerRef = Cast<AStudyCharacter>(GetOwner());
	if(PlayerRef)
	{
		USkillTreeComponent* Component = Cast<USkillTreeComponent>(PlayerRef->FindComponentByClass(USkillTreeComponent::StaticClass()));
		if(Component)
		{
			// Calculate Cool Down and returns it as a Text
			Server_GetCoolDownText();
			// Calculated the CoolDown percentage based on it's current position/lenght
			Server_CalculateCoolDownPercentage();
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

void AMasterSkill::Server_CalculateCoolDownPercentage_Implementation()
{
	if(GetLocalRole() == ROLE_Authority)
	{
		// Calculated the CoolDown percentage based on it's current position/lenght
		float playbackPosition = CoolDownTimeLine->GetPlaybackPosition();
		float timelineLength = CoolDownTimeLine->GetTimelineLength();
		coolDownPercentage = 1.f - (playbackPosition/timelineLength);
	} else
	{
		Client_CalculateCoolDownPercentage();
	}
}


bool AMasterSkill::Server_CalculateCoolDownPercentage_Validate()
{
	return true;
}


void AMasterSkill::Client_CalculateCoolDownPercentage_Implementation()
{
	Server_CalculateCoolDownPercentage();
}

void AMasterSkill::Server_GetCoolDownText_Implementation()
{
	if(GetLocalRole() == ROLE_Authority)
	{
		// Calculate CoolDown in seconds based on his length - his current value
		const TEnumAsByte<ERoundingMode> RoundingMode = ERoundingMode::HalfToEven;
		coolDownInSeconds = UKismetTextLibrary::Conv_FloatToText(
            (CoolDownTimeLine->GetTimelineLength()-CoolDownTimeLine->GetPlaybackPosition()),
            RoundingMode, false,false, 1, 324,
            0, 0);
	} else
	{
		Client_GetCoolDownText();
	}
}


bool AMasterSkill::Server_GetCoolDownText_Validate()
{
	return true;
}


void AMasterSkill::Client_GetCoolDownText_Implementation()
{
	Server_GetCoolDownText();
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
			Server_PlayCoolDownFromStart();
		} else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to Cast to Component on Master Skill"))
		}
	} else
	{
		UE_LOG(LogTemp, Warning, TEXT("Fail to Cast to Player Character on Master Skill"))
	}
}

void AMasterSkill::Server_PlayCoolDownFromStart_Implementation()
{
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
		Client_PlayCoolDownFromStart();
	}
}


bool AMasterSkill::Server_PlayCoolDownFromStart_Validate()
{
	return true;
}


void AMasterSkill::Client_PlayCoolDownFromStart_Implementation()
{
	Server_PlayCoolDownFromStart();
}
