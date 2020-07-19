// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillDescription.h"
#include "Skill_Slot_Defaults.h"
#include "StudyPC.h"
#include "MasterSkill.h"
#include "StudyCharacter.h"
#include "StudyPlayerState.h"
#include "SkillTreeComponent.h"
#include "Engine/DataTable.h"
#include "Engine/Classes/Kismet/KismetSystemLibrary.h"

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
				bHasTheProperLvl = StateRef->CharacterStats.CurrentLevel >= SkillTreeInfo.GoldLevelRequired;
				// Has the amount of gold required to buy the skill?
				bHasTheProperGold = StateRef->CharacterStats.GoldAmount >= SkillTreeInfo.PriceToUnlock;
				// Has unlocked the skill on the lower tree lvl already?
				bHasUnlockedRequiredSkill = (SkillTreeInfo.RequiredSkillToUnlock.ToString() == "None") ||
					(PCRef->CharacterSkills.Contains(SkillTreeInfo.RequiredSkillToUnlock));
			} else {UE_LOG(LogTemp, Error, TEXT("Failed to Cast to Player State to get Character Stats Information"));}
		}
	} else {UE_LOG(LogTemp, Error, TEXT("Failed to Cast to Player Controller to Get Skill Information"));}

	SkillInfo = getSkillInfo(SkillTreeInfo.SkillClass);
	
	// Fill UI with Data
	SkillDescription->SetText(SkillInfo.Description);
	// if already unlocked, just show an equip button
	if(bCanEquip)
	{
		SkillRequirements->SetText(FText::FromString("Consumes " + FString::FromInt(SkillInfo.MPCost)
			+ " of MP for each usage"));
		UnlockButtonText->SetText(FText::FromString("Equip"));
	}
	// if not unlocked, check if the user can unlock the skill
	// if he can unlock, allow him to do so
	else if(bHasTheProperLvl && bHasTheProperGold && bHasUnlockedRequiredSkill)
	{
		SkillRequirements->SetText(FText::FromString("Cost: " + FString::FromInt(SkillTreeInfo.PriceToUnlock)));
		UnlockButtonText->SetText(FText::FromString("Unlock"));
	}
	// If user has the proper lvl but didn't have unlocked the required skill
	else if(!bHasUnlockedRequiredSkill)
	{
		SkillRequirements->SetText(FText::FromString("Requires skill " +
			getSkillInfo(getSkillTreeDetails(SkillTreeInfo.RequiredSkillToUnlock).SkillClass).Name.ToString() +
		" to be unlocked"));
		UnlockButtonText->SetText(FText::FromString("Close"));
	}
	// If the user doesn't have the proper lvl
	else if(!bHasTheProperLvl)
	{
		SkillRequirements->SetText(FText::FromString("Requires lvl " +
			FString::FromInt(SkillTreeInfo.GoldLevelRequired)));
		UnlockButtonText->SetText(FText::FromString("Close"));
	}
	// otherwise if user simply doesn't have the amount of gold to unlock the skill
	else
	{
		SkillRequirements->SetText(FText::FromString("Cost: " + FString::FromInt(SkillTreeInfo.PriceToUnlock)));
		UnlockButtonText->SetText(FText::FromString("Close"));
	}
}

void USkillDescription::OnUnlockClicked()
{
	if(this->GetOwningPlayerPawn())
	{
		USkillTreeComponent* Component = Cast<USkillTreeComponent>(
            this->GetOwningPlayerPawn()->FindComponentByClass(USkillTreeComponent::StaticClass()));
		if(Component)
		{
			if(bCanEquip)
			{
				// @TODO: something to equip the skill
			}
			else if (bHasTheProperGold && bHasTheProperLvl && bHasUnlockedRequiredSkill)
			{
				AStudyPC* PCRef = GetCustomController();
				Component->UnlockSkill(PCRef);
			}
			this->RemoveFromParent();
		}
	}
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

FSkilDataTable USkillDescription::getSkillTreeDetails(FName SkillRow)
{
	// load data table
	static const FString ContextCurrent(TEXT("Current Item Details"));
	FSkilDataTable* row = SkillSlotRef->DetailsTable->FindRow<FSkilDataTable>(
		SkillRow, ContextCurrent, true);
	if(row) return *(row);
	return FSkilDataTable();
}

FSkillDetails USkillDescription::getSkillInfo(TSubclassOf<AMasterSkill> SkillClass)
{
	if(UKismetSystemLibrary::IsValidClass(SkillClass))
    {
    	AMasterSkill* DefaultActor = Cast<AMasterSkill>(SkillClass->GetDefaultObject(true));
    	if(DefaultActor)
    	{
    		UE_LOG(LogTemp, Log, TEXT("Valid Master Skill"));
    		return DefaultActor->SkillDetails;
    	}
    }
	return FSkillDetails();
}