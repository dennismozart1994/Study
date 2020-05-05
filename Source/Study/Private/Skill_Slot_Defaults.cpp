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

	DefaultStyle.Normal = Normal;
	DefaultStyle.Hovered = Hovered;
	DefaultStyle.Pressed = Normal;

	LockerImage = LoadObject<UTexture2D>(this, TEXT("Texture2D'/Game/UI/Actions/SkillTree/Images/padlock-open.padlock-open'"));
	if(LockerImage != nullptr) SkillLocker->SetBrushFromTexture(LockerImage);

	if(SkillSlot != nullptr) SkillSlot->SetStyle(DefaultStyle);

	// Set Skill Thumbnail image
	FSkilDataTable SkillDetails = getSkillDetails();
	if(SkillDetails.SkillThumbnail != nullptr && SkillThumbnail != nullptr) {
		SkillThumbnail->SetBrushFromTexture(SkillDetails.SkillThumbnail);
		UE_LOG(LogTemp, Log, TEXT("Set Skill Thumbnail"));
	}

	// Set button as Enabled if skill is already unlocked
	AStudyPC* controllerRef = GetCustomController();
	if(controllerRef) {
		if(controllerRef->CharacterSkills.Contains(SkillRow)){
			SkillSlot->bIsEnabled = true;
			SkillLocker->SetVisibility(ESlateVisibility::Hidden);
			UE_LOG(LogTemp, Log, TEXT("Successfully enabled the slot button"));
		} else {
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

}

FSkilDataTable USkill_Slot_Defaults::getSkillDetails()
{
	// load data table
	static const FString ContextCurrent(TEXT("Current Item Details"));
	FSkilDataTable* row = DetailsTable->FindRow<FSkilDataTable>(SkillRow, ContextCurrent, true);
	if(row) return *(row);
	return ACustomVariables::createSkillStruct();
}


