// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillDescription.h"
#include "Skill_Slot_Defaults.h"
#include "StudyPC.h"
#include "StudyCharacter.h"
#include "StudyPlayerState.h"

// UI Bindings
bool USkillDescription::Initialize()
{
	bool success = Super::Initialize();
	if(!success) return false;
	
	if(!ensure(UnlockButton != nullptr) && UnlockButtonText != nullptr) return false;
	UnlockButton->OnClicked.AddDynamic(this, &USkillDescription::OnUnlockClicked);
	return success;
}

void USkillDescription::NativeConstruct()
{
	Super::NativeConstruct();

	// Get needed Information from Skill Info
	AStudyPC* PCRef = GetCustomController();
	if(PCRef)
	{
		// If already Unlocked, just allow the player to equip the skill into the proper session
		if(PCRef->CharacterSkills.Contains(SkillSlotRef->SkillRow))
		{
			UE_LOG(LogTemp, Log, TEXT("Skill already unlocked, select a slot to equip"));
			bCanEquip = true;
		}
		// Otherwise, Unlock the Skill
		else
		{
			AStudyPlayerState* StateRef = PCRef->GetPersonalPlayerState();
			if(StateRef != nullptr)
			{
				// cannot equip since it's unlocked
				bCanEquip = false;
				// Has the minimum level to unlock the skill?
				bHasTheProperLvl = StateRef->CharacterStats.CurrentLevel >= SkillInfo.GoldLevelRequired;
				// Has the amount of gold required to buy the skill?
				bHasTheProperGold = StateRef->CharacterStats.GoldAmount >= SkillInfo.PriceToUnlock;
				// Has unlocked the skill on the lower tree lvl already?
				bHasUnlockedRequiredSkill = (SkillInfo.RequiredSkillToUnlock.ToString() == "None") ||
					(PCRef->CharacterSkills.Contains(SkillInfo.RequiredSkillToUnlock));
			} else {UE_LOG(LogTemp, Error, TEXT("Failed to Cast to Player State to get Character Stats Information"));}
		}
	} else {UE_LOG(LogTemp, Error, TEXT("Failed to Cast to Player Controller to Get Skill Information"));}

	// Fill UI with Data
	SkillDescription->SetText(SkillInfo.Description);
	// if already unlocked, just show an equip button
	if(bCanEquip)
	{
		UnlockButtonText->SetText(FText::FromString("Equip"));
	}
	// if not unlocked, check if the user can unlock the skill
	// if he can unlock, allow him to do so
	else if(bHasTheProperLvl && bHasTheProperGold && bHasUnlockedRequiredSkill)
	{
		SkillRequirements->SetText(FText::FromString("Cost: " + FString::FromInt(SkillInfo.PriceToUnlock)));
		UnlockButtonText->SetText(FText::FromString("Unlock"));
	}
	// If user has the proper lvl but didn't have unlocked the required skill
	else if(!bHasUnlockedRequiredSkill)
	{
		SkillRequirements->SetText(FText::FromString("Requires skill " +
            SkillInfo.RequiredSkillToUnlock.ToString() + " to be unlocked"));
		UnlockButtonText->SetText(FText::FromString("Close"));
	}
	// If the user doesn't have the proper lvl
	else if(!bHasTheProperLvl)
	{
		SkillRequirements->SetText(FText::FromString("Requires lvl " +
			FString::FromInt(SkillInfo.GoldLevelRequired)));
		UnlockButtonText->SetText(FText::FromString("Close"));
	}
	// otherwise if user simply doesn't have the amount of gold to unlock the skill
	else
	{
		SkillRequirements->SetText(FText::FromString("Cost: " + FString::FromInt(SkillInfo.PriceToUnlock)));
		UnlockButtonText->SetText(FText::FromString("Close"));
	}
}

void USkillDescription::OnUnlockClicked()
{
	if(bCanEquip)
	{
		// TODO: something to equip the skill
	}
	else if (bHasTheProperGold && bHasTheProperLvl && bHasUnlockedRequiredSkill)
	{
		AStudyPC* PCRef = GetCustomController();
		if(PCRef != nullptr)
		{
			PCRef->Server_UnlockSkill(PCRef, SkillSlotRef->SkillRow);
			SkillSlotRef->SkillLocker->SetVisibility(ESlateVisibility::Hidden);
		} else {UE_LOG(LogTemp, Error, TEXT("Failed to get the Player Controller Reference"));}
	}

	this->RemoveFromParent();
}

// Custom Getters
AStudyPC* USkillDescription::GetCustomController()
{
	return Cast<AStudyPC>(GetOwningPlayer());
}

AStudyCharacter* USkillDescription::GetCustomCharacter()
{
	return Cast<AStudyCharacter>(GetOwningPlayerPawn());
}

AStudyPlayerState* USkillDescription::GetCustomPlayerState()
{
	return Cast<AStudyPlayerState>(GetOwningPlayer()->PlayerState);
}