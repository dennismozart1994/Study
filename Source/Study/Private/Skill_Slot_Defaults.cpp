// Fill out your copyright notice in the Description page of Project Settings.


#include "Skill_Slot_Defaults.h"
#include "StudyPC.h"
#include "StudyCharacter.h"
#include "StudyPlayerState.h"
#include "Engine/DataTable.h"
#include "SkillDescription.h"

// Widget Helpers
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

// Math and debugging includes
#include "Engine/Classes/Kismet/KismetSystemLibrary.h"
#include "Engine/Classes/Kismet/KismetMathLibrary.h"

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
			SkillLocker->SetVisibility(ESlateVisibility::Hidden);
			UE_LOG(LogTemp, Log, TEXT("Successfully enabled the slot button"));
		} else {
			DefaultStyle.Normal = Hovered;
			DefaultStyle.Hovered = Normal;
			DefaultStyle.Pressed = Hovered;
			SkillSlot->SetStyle(DefaultStyle);
			SkillLocker->SetVisibility(ESlateVisibility::Visible);
			UE_LOG(LogTemp, Warning, TEXT("Skill is still unlocked, locker image will be presented"));
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

// Present skill details, all checks for unlock or not are done in there.
void USkill_Slot_Defaults::OnSlotClicked()
{
	// remove all detailing widgets that could have beeing presented by previous clicks
	TArray<UUserWidget*> WidgetsBeingPresented;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), WidgetsBeingPresented, wSkillDetails, true);
	for(UUserWidget* CustomDetails : WidgetsBeingPresented)
	{
		CustomDetails->RemoveFromParent();
	}
	
	// create a new details widget using the skill information
	FSkilDataTable row = getSkillDetails();
	SkillDetailsWG = CreateWidget<USkillDescription>(GetOwningPlayer(), wSkillDetails);
	SkillDetailsWG->SkillSlotRef = this;
	SkillDetailsWG->SkillInfo = row;

	UWidgetLayoutLibrary* WidgetLayoutLibrary;
	WidgetLayoutLibrary = NewObject<UWidgetLayoutLibrary>(UWidgetLayoutLibrary::StaticClass());

	if (WidgetLayoutLibrary)
	{
		FVector2D ViewportPosition = WidgetLayoutLibrary->GetMousePositionOnViewport(GetWorld());
		if (SkillDetailsWG)
		{
			SkillDetailsWG->AddToViewport(1);
			float Y;
			float X;
			UKismetMathLibrary::BreakVector2D(ViewportPosition, X, Y);
			X += 30.f;
			Y += -90.f;
			FVector2D Position = UKismetMathLibrary::MakeVector2D(X, Y);

			SkillDetailsWG->SetPositionInViewport(Position, false);
		}
	}
}

FSkilDataTable USkill_Slot_Defaults::getSkillDetails()
{
	// load data table
	static const FString ContextCurrent(TEXT("Current Item Details"));
	FSkilDataTable* row = DetailsTable->FindRow<FSkilDataTable>(SkillRow, ContextCurrent, true);
	if(row) return *(row);
	return ACustomVariables::createSkillStruct();
}


