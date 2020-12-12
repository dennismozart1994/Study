// Fill out your copyright notice in the Description page of Project Settings.

#include "StudyPC.h"
#include "Net/UnrealNetwork.h"
#include "StudyPlayerState.h"
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

	// Initialize Equipped Skills with null values
	for(int i=1; i<=6; i++)
	{
		WarriorSkills.Add(FName("None"));
		ArcherSkills.Add(FName("None"));
		MageSkills.Add(FName("Name"));
	}
	
}

void AStudyPC::Server_UnlockSkill_Implementation(AStudyPC* Controller, FName Skill)
{
	if(Controller) {
		AStudyPlayerState* State = Cast<AStudyPlayerState>(Controller->PlayerState);
		if(State) {
			// If is the server who's calling the unlock and the user hasn't already unlocked the Skill
			if((State->GetLocalRole() == ROLE_Authority || (State->GetLocalRole() == ROLE_Authority && State->GetRemoteRole() < ROLE_AutonomousProxy)) && !Controller->CharacterSkills.Contains(Skill)
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

void AStudyPC::Server_EquipSkill_Implementation(AStudyPC* Controller, FName Skill, int32 SlotIndex)
{
	const FSkilDataTable SkillDetails = getSkillDetails(Skill);
	switch (SkillDetails.TreeClass)
	{
		case ESkillClass::SC_Warrior: WarriorSkills[SlotIndex] = Skill;
		break;
		case ESkillClass::SC_Archier: ArcherSkills[SlotIndex] = Skill;
		break;
		case ESkillClass::SC_Magician: MageSkills[SlotIndex] = Skill;
		break;
		default: UE_LOG(LogTemp, Error, TEXT("Invalid Skill Class"));
		break;
	}
}

bool AStudyPC::Server_EquipSkill_Validate(AStudyPC* Controller, FName Skill, int32 SlotIndex)
{
	return true;
}

void AStudyPC::Client_EquipSkill_Implementation(AStudyPC* Controller, FName Skill, int32 SlotIndex)
{
	Server_EquipSkill_Implementation(Controller, Skill, SlotIndex);
}

TArray<FName> AStudyPC::GetSkillArray(ESkillClass Tree) const
{
	TArray<FName> Empty;
	switch (Tree)
	{
		case ESkillClass::SC_Warrior:
			UE_LOG(LogTemp, Log, TEXT("Grabbed Warrior Skills"));
			return WarriorSkills;
		case ESkillClass::SC_Archier:
			UE_LOG(LogTemp, Log, TEXT("Grabbed Archer Skills"));
			return ArcherSkills;
		case ESkillClass::SC_Magician:
			UE_LOG(LogTemp, Log, TEXT("Grabbed Mage Skills"));
			return MageSkills;
		default:
			UE_LOG(LogTemp, Error, TEXT("Returned Empty Skills Array"));
			return Empty;
	}
}


FSkilDataTable AStudyPC::getSkillDetails(FName rowName)
{
	// load data table
	static const FString ContextCurrent(TEXT("Current Item Details"));
	FSkilDataTable* row = SkillTable->FindRow<FSkilDataTable>(rowName, ContextCurrent, true);
	if(row) return *(row);
	return FSkilDataTable();
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
	 DOREPLIFETIME(AStudyPC, WarriorSkills);
	 DOREPLIFETIME(AStudyPC, ArcherSkills);
	 DOREPLIFETIME(AStudyPC, MageSkills);
}
