// Fill out your copyright notice in the Description page of Project Settings.

#include "StudyPC.h"
#include "Net/UnrealNetwork.h"
#include "StudyPlayerState.h"
#include "Engine/Engine.h"
#include "Engine/DataTable.h"

AStudyPC::AStudyPC()
{
    bReplicates = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
	bEnableTouchEvents = false;
	ClickEventKeys.Add(EKeys::LeftMouseButton);
	ClickEventKeys.Add(EKeys::RightMouseButton);
	DefaultClickTraceChannel = ECollisionChannel::ECC_Camera;
}

void AStudyPC::Server_UnlockSkill_Implementation(AStudyPC* Controller, FName Skill)
{
	if(Controller) {
		AStudyPlayerState* State = Cast<AStudyPlayerState>(Controller->PlayerState);
		if(State) {
			// If is the server who's calling the unlock and the user hasn't already unlocked the Skill
			if(State->GetLocalRole() == ROLE_Authority && !Controller->CharacterSkills.Contains(Skill)
					&& !Skill.ToString().IsEmpty()) {
				State->CharacterStats.GoldAmount-=getSkillDetails(Skill).PriceToUnlock;
				Controller->CharacterSkills.Add(Skill);
			} else {Client_UnlockSkill(Controller, Skill);}
		} else {UE_LOG(LogTemp, Error, TEXT("Error trying to cast to Player State"));}
	} else {UE_LOG(LogTemp, Error, TEXT("Invalid Controller passed as a reference to the function"));}
}

bool AStudyPC::Server_UnlockSkill_Validate(AStudyPC* Controller, FName Skill)
{
	return true;
}

void AStudyPC::Client_UnlockSkill_Implementation(AStudyPC *Controller, FName Skill)
{
	Server_UnlockSkill(Controller, Skill);
}


FSkilDataTable AStudyPC::getSkillDetails(FName rowName)
{
	// load data table
	static const FString ContextCurrent(TEXT("Current Item Details"));
	FSkilDataTable* row = SkillTable->FindRow<FSkilDataTable>(rowName, ContextCurrent, true);
	if(row) return *(row);
	return ACustomVariables::createSkillStruct();
}

AStudyPlayerState* AStudyPC::GetPersonalPlayerState()
{
    AStudyPlayerState* MyPlayerState = Cast<AStudyPlayerState>(PlayerState);
    return MyPlayerState;
}

void AStudyPC::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
 {
     Super::GetLifetimeReplicatedProps(OutLifetimeProps);
 
     DOREPLIFETIME(AStudyPC, Inventory);
	 DOREPLIFETIME(AStudyPC, InventoryItems);
	 DOREPLIFETIME(AStudyPC, CharacterSkills);
}
