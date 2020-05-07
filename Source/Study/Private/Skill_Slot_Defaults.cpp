// Fill out your copyright notice in the Description page of Project Settings.


#include "Skill_Slot_Defaults.h"
#include "StudyPC.h"
#include "StudyCharacter.h"
#include "StudyPlayerState.h"
#include "Engine/DataTable.h"
#include "Engine/Engine.h"

//////////////////////////////////////////////////// NATIVE OVERRIDES ////////////////////////////////////////////////////////
bool USkill_Slot_Defaults::Initialize()
{
	bool success = Super::Initialize();
	if(!success) return false;

	if(!ensure(SkillSlot != nullptr) && !ensure(SkillThumbnail != nullptr)) return false;
	SkillSlot->OnClicked.AddDynamic(this, &USkill_Slot_Defaults::OnSlotClicked);
	return success;
}

void USkill_Slot_Defaults::NativeConstruct()
{
	Super::NativeConstruct();

	// Set Defualt Button Style
	Normal.DrawAs = ESlateBrushDrawType::Image;
	Normal.Tiling = ESlateBrushTileType::NoTile;
	Normal.Mirroring = ESlateBrushMirrorType::NoMirror;
	Normal.ImageSize = FVector2D(32.f, 32.f);
	Normal.SetResourceObject(SlotBackground);
	Normal.TintColor = FSlateColor(FLinearColor(1.f, 1.f, 1.f, 1.f));

	Hovered.DrawAs = ESlateBrushDrawType::Image;
	Hovered.Tiling = ESlateBrushTileType::NoTile;
	Hovered.Mirroring = ESlateBrushMirrorType::NoMirror;
	Hovered.ImageSize = FVector2D(32.f, 32.f);
	Hovered.SetResourceObject(SlotBackground);
	Hovered.TintColor = FSlateColor(FLinearColor(0.447917f, 0.447917f, 0.447917f, 1.f));

	LockerImage = LoadObject<UTexture2D>(this, TEXT("Texture2D'/Game/UI/Actions/SkillTree/Images/padlock-open.padlock-open'"));
	if(LockerImage != nullptr) SkillLocker->SetBrushFromTexture(LockerImage);

	// Set Skill Thumbnail image
	FSkilDataTable SkillDetails = getSkillDetails();
	if(SkillDetails.SkillThumbnail != nullptr && SkillThumbnail != nullptr) {
		SkillThumbnail->SetBrushFromTexture(SkillDetails.SkillThumbnail);
		UE_LOG(LogTemp, Log, TEXT("Set Skill Thumbnail"));
	}

	// Set button as Enabled if skill is already unlocked
	AStudyPC* controllerRef = GetCustomController();
	if(controllerRef) {
		if(controllerRef->CharacterSkills.Contains(SkillRow) && SkillSlot != nullptr){
			DefaultStyle.Normal = Normal;
			DefaultStyle.Hovered = Hovered;
			DefaultStyle.Pressed = Normal;
			SkillSlot->SetStyle(DefaultStyle);
			SkillSlot->bIsEnabled = true;
			SkillLocker->SetVisibility(ESlateVisibility::Hidden);
			UE_LOG(LogTemp, Log, TEXT("Successfully enabled the slot button"));
		} else {
			DefaultStyle.Normal = Hovered;
			DefaultStyle.Hovered = Normal;
			DefaultStyle.Pressed = Hovered;
			SkillSlot->SetStyle(DefaultStyle);
			SkillSlot->bIsEnabled = false;
			SkillLocker->SetVisibility(ESlateVisibility::Visible);
			UE_LOG(LogTemp, Error, TEXT("Failed to find Skill in the list, disabled slot button"));
		}
	}
}

//////////////////////////////////////////////////// FUNCTIONS //////////////////////////////////////////////////////////
AStudyPC* USkill_Slot_Defaults::GetCustomController()
{
	return Cast<AStudyPC>(GetOwningPlayer());
}

AStudyCharacter* USkill_Slot_Defaults::GetCustomCharacter()
{
	return Cast<AStudyCharacter>(GetOwningPlayerPawn());
}

AStudyPlayerState* USkill_Slot_Defaults::GetCustomPlayerState()
{
	return Cast<AStudyPlayerState>(GetOwningPlayer()->PlayerState);
}

void USkill_Slot_Defaults::OnSlotClicked()
{
	AStudyPC* PCRef = GetCustomController();
	if(PCRef != nullptr) {
		// If already Unlocked, just allow the player to equip the skill into the proper session
		if(PCRef->CharacterSkills.Contains(SkillRow)){
			UE_LOG(LogTemp, Log, TEXT("Skill already unlocked, select a slot to equip"));
		// Otherwise, Unlock the Skill
		} else {
			AStudyPlayerState* StateRef = PCRef->GetPersonalPlayerState();
			if(StateRef != nullptr) {
				// Only Unlock it after confirmation, but for testing we're not gonna do that
				// Only unlock if meet skill criteria
				FSkilDataTable row = getSkillDetails();
				// Has the minimum level to unlock the skill?
				bool bHasTheProperLvl = StateRef->CharacterStats.CurrentLevel >= row.GoldLevelRequired;
				// Has the amount of gold required to buy the skill?
				bool bHasTheProperGold = StateRef->CharacterStats.GoldAmount >= row.PriceToUnlock;
				// Has unlocked the skill on the lower tree lvl already?
				bool bHasUnlockedRequiredSkill = (row.RequiredSkillToUnlock.ToString() == "None") || (PCRef->CharacterSkills.Contains(row.RequiredSkillToUnlock));
				if(bHasTheProperLvl && bHasTheProperGold && bHasUnlockedRequiredSkill) {
					PCRef->Server_UnlockSkill(PCRef, SkillRow);
					SkillLocker->SetVisibility(ESlateVisibility::Hidden);
					UE_LOG(LogTemp, Log, TEXT("Unlocked %s"), *SkillRow.ToString());
				} else {UE_LOG(LogTemp, Error, TEXT("Either user doesn't have the required lvl or the proper amount of money"));}
			} else {UE_LOG(LogTemp, Error, TEXT("Failed to Cast to the PlayerState"));}
		}
	} else {UE_LOG(LogTemp, Error, TEXT("Failed to Cast to the Player Controller"));}
}

FSkilDataTable USkill_Slot_Defaults::getSkillDetails()
{
	// load data table
	static const FString ContextCurrent(TEXT("Current Item Details"));
	FSkilDataTable* row = DetailsTable->FindRow<FSkilDataTable>(SkillRow, ContextCurrent, true);
	if(row) return *(row);
	return ACustomVariables::createSkillStruct();
}


