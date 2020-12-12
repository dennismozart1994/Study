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
		FFormatNamedArguments Args;
		Args.Add(TEXT("ConsumesString"), FText::FromStringTable(COMMON_STRING_TABLE, "Consumes"));
		Args.Add(TEXT("MPCost"), FText::AsNumber(SkillInfo.MPCost));
		Args.Add(TEXT("OfString"), FText::FromStringTable(COMMON_STRING_TABLE, "Of"));
		Args.Add(TEXT("MPString"), FText::FromStringTable(COMMON_STRING_TABLE, "MP"));
		Args.Add(TEXT("ForEachUsageString"), FText::FromStringTable(COMMON_STRING_TABLE, "ForEachUsage"));

		const FText ButtonDescription = FText::Format(NSLOCTEXT("SkillDescriptionUI", "MagicPointsConsumeString",
			"{ConsumesString} {MPCost} {OfString} {MPString} {ForEachUsageString}"), Args);
		
		SkillRequirements->SetText(ButtonDescription);
		UnlockButtonText->SetText(FText::FromStringTable(COMMON_STRING_TABLE, "Equip"));
		PriceText->SetText(FText::FromString(""));
	}
	// if not unlocked, check if the user can unlock the skill
	// if he can unlock, allow him to do so
	else if(bHasTheProperLvl && bHasTheProperGold && bHasUnlockedRequiredSkill)
	{
		SkillRequirements->SetText(FText::Format(
			NSLOCTEXT("SkillDescriptionUI", "MagicPointsComsumptionString",
            "{0} {1} {2} {3} {4}"),
            FText::FromStringTable(COMMON_STRING_TABLE, "Consumes"),
            FText::AsNumber(SkillInfo.MPCost),
            FText::FromStringTable(COMMON_STRING_TABLE, "Of"),
            FText::FromStringTable(COMMON_STRING_TABLE, "MP"),
            FText::FromStringTable(COMMON_STRING_TABLE, "ForEachUsage")
		));
		UnlockButtonText->SetText(FText::FromStringTable(COMMON_STRING_TABLE,"Unlock"));
		PriceText->SetText(FText::Format(NSLOCTEXT("SkillDescriptionUI", "PriceTextString", "{0}G"), FText::AsNumber(SkillTreeInfo.PriceToUnlock)));
	}
	// If user has the proper lvl but didn't have unlocked the required skill
	else if(!bHasUnlockedRequiredSkill)
	{
		FFormatNamedArguments Args;
		Args.Add(TEXT("RequiresString"), FText::FromStringTable(COMMON_STRING_TABLE, "Requires"));
		Args.Add(TEXT("SkillString"), FText::FromStringTable(COMMON_STRING_TABLE, "Skill"));
		Args.Add(TEXT("SkillName"), getSkillInfo(getSkillTreeDetails(SkillTreeInfo.RequiredSkillToUnlock).SkillClass).Name);
		Args.Add(TEXT("ToBeString"), FText::FromStringTable(COMMON_STRING_TABLE, "ToBe"));
		Args.Add(TEXT("UnlockedString"), FText::FromStringTable(COMMON_STRING_TABLE, "Unlocked"));
		
		const FText ButtonText = FText::Format(NSLOCTEXT("SkillDescriptionUI",
            "RequiresSkillDescription",
            "{RequiresString} {SkillString} {SkillName} {ToBeString} {UnlockedString}")
            , Args);
		SkillRequirements->SetText(ButtonText);
		UnlockButtonText->SetText(FText::FromStringTable(COMMON_STRING_TABLE, "Close"));
		PriceText->SetText(FText::FromString(""));
	}
	// If the user doesn't have the proper lvl
	else if(!bHasTheProperLvl)
	{
		FFormatNamedArguments Args;
		Args.Add(TEXT("RequiresString2"), FText::FromStringTable(COMMON_STRING_TABLE, "Requires"));
		Args.Add(TEXT("LevelString"), FText::FromStringTable(COMMON_STRING_TABLE, "Level"));
		Args.Add(TEXT("LevelRequired"), FText::AsNumber(SkillTreeInfo.GoldLevelRequired));

		SkillRequirements->SetText(FText::Format(NSLOCTEXT("SkillDescriptionUI",
            "RequiresLvlDescription",
            "{RequiresString2} {LevelString} {LevelRequired}"),
            Args));
		UnlockButtonText->SetText(FText::FromStringTable(COMMON_STRING_TABLE, "Close"));
		PriceText->SetText(FText::FromString(""));
	}
	// otherwise if user simply doesn't have the amount of gold to unlock the skill
	else
	{
		FFormatNamedArguments Args;
		Args.Add(TEXT("CostString2"), FText::FromStringTable(COMMON_STRING_TABLE, "Cost"));
		Args.Add(TEXT("PriceToUnlock"), FText::AsNumber(SkillTreeInfo.PriceToUnlock));
		
		SkillRequirements->SetText(FText::Format(NSLOCTEXT("SkillDescriptionUI",
            "CostDescription2",
            "{CostString2}: {PriceToUnlock}G"),
            Args));
		UnlockButtonText->SetText(FText::FromStringTable(COMMON_STRING_TABLE, "Close"));
		PriceText->SetText(FText::FromString(""));
	}
}

void USkillDescription::OnUnlockClicked()
{
	if(this->GetOwningPlayerPawn())
	{
		AStudyCharacter* PlayerRef = Cast<AStudyCharacter>(this->GetOwningPlayerPawn());
		if(PlayerRef)
		{
			USkillTreeComponent* Component = Cast<USkillTreeComponent>(
            PlayerRef->FindComponentByClass(USkillTreeComponent::StaticClass()));
			if(Component)
			{
				if(bCanEquip)
				{
					// @TODO: something to equip the skill
					Component->StartEquipSkill();
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