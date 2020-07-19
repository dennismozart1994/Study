// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillTreeComponent.h"
#include "StudyCharacter.h"
#include "StudyPC.h"
#include "MasterSkill.h"
#include "Skill3DPreview.h"
#include "Blueprint/UserWidget.h"
#include "SkillDescription.h"
#include "Skill_Slot_Defaults.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"

// Sets default values for this component's properties
USkillTreeComponent::USkillTreeComponent()
{
}


// Called when the game starts
void USkillTreeComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

FSkilDataTable USkillTreeComponent::GetSkillTreeItem(FName RowName)
{
	if(DetailsTable)
	{
		// load data table
		static const FString ContextCurrent(TEXT("Current Item Details"));
		FSkilDataTable* row = DetailsTable->FindRow<FSkilDataTable>(RowName, ContextCurrent, true);
		// If row was found, return it
		if(row)
		{
			return *(row);
		}
		UE_LOG(LogTemp, Warning, TEXT("Skill Row provided was not found inside the Data Table"))
	} else
	{
		UE_LOG(LogTemp, Error, TEXT("No Skill Data Table was found at the Skill Component"));
	}
	return FSkilDataTable();
}


FSkillDetails USkillTreeComponent::GetSkillInfo()
{
	if(SkillDetailsRef)
	{
		if(SkillDetailsRef->SkillSlotRef)
		{
			const FSkilDataTable row = GetSkillTreeItem(SkillDetailsRef->SkillSlotRef->SkillRow);
			// If row was found, return the class defaults of the skill
			if(UKismetSystemLibrary::IsValidClass(row.SkillClass)) {
				AMasterSkill* DefaultActor = Cast<AMasterSkill>(row.SkillClass->GetDefaultObject(true));
				if(DefaultActor)
				{
					UE_LOG(LogTemp, Log, TEXT("Valid Master Skill"));
					return DefaultActor->SkillDetails;
				}
			}
		} else
		{
			UE_LOG(LogTemp, Error, TEXT("Skill Slot Widget Reference is Null"))
        }
	} else
	{
		UE_LOG(LogTemp, Error, TEXT("Skill Details Widget Reference is Null"))
    }
	
	return FSkillDetails();
}


void USkillTreeComponent::RemoveAllPreviewWidgets()
{
	// remove all detailing widgets that could have being presented by previous clicks
	TArray<UUserWidget*> WidgetsBeingPresented;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), WidgetsBeingPresented, SkillDetailsClass, true);
	for(UUserWidget* CustomDetails : WidgetsBeingPresented)
	{
		CustomDetails->RemoveFromParent();
	}
}

void USkillTreeComponent::CreatePreviewWidget(USkill_Slot_Defaults* SlotRef)
{
	AStudyCharacter* CharacterRef = Cast<AStudyCharacter>(GetOwner());
	if(UKismetSystemLibrary::IsValidClass(SkillDetailsClass) && CharacterRef)
	{
		AStudyPC* OwningController = Cast<AStudyPC>(CharacterRef->GetController());
		if(OwningController)
		{
			// create a new details widget using the skill information
			SkillDetailsRef = CreateWidget<USkillDescription>(OwningController, SkillDetailsClass);
			SkillDetailsRef->SkillSlotRef = SlotRef;
			SkillDetailsRef->SkillTreeInfo = GetSkillTreeItem(SlotRef->SkillRow);

			UWidgetLayoutLibrary* WidgetLayoutLibrary;
			WidgetLayoutLibrary = NewObject<UWidgetLayoutLibrary>(UWidgetLayoutLibrary::StaticClass());

			if (WidgetLayoutLibrary)
			{
				FVector2D ViewportPosition = WidgetLayoutLibrary->GetMousePositionOnViewport(GetWorld());
				if (SkillDetailsRef)
				{
					SkillDetailsRef->AddToViewport(1);
					float Y;
					float X;
					UKismetMathLibrary::BreakVector2D(ViewportPosition, X, Y);
					X += 30.f;
					Y += -90.f;
					FVector2D Position = UKismetMathLibrary::MakeVector2D(X, Y);

					SkillDetailsRef->SetPositionInViewport(Position, false);
				}
			}
			PlayPreview();
		}
	} else
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid Skill Detail Widget Class assigned as default on Skill Tree Component"))
	}
}

void USkillTreeComponent::PlayPreview()
{
	// If we have a montage to play
	if(GetSkillInfo().MontageToPlay)
	{
		// get all preview actors (it should have only 1 per lvl
		TArray<AActor*> PreviewActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASkill3DPreview::StaticClass(), PreviewActors);
		for(AActor* PreviewActor : PreviewActors)
		{
			ASkill3DPreview* Preview3D = Cast<ASkill3DPreview>(PreviewActor);
			// if cast is successful, then update the preview mesh and play the animation
			if (Preview3D)
			{
				Preview3D->UpdateArmorSet();
				Preview3D->PlaySkillAnimation(GetSkillInfo().MontageToPlay);
			}
		}
	}
}

void USkillTreeComponent::UnlockSkill(AStudyPC* Controller)
{
	if(Controller != nullptr && SkillDetailsRef && SkillDetailsRef->SkillSlotRef)
	{
		Controller->Server_UnlockSkill(Controller, SkillDetailsRef->SkillSlotRef->SkillRow);
		SkillDetailsRef->SkillSlotRef->SkillLocker->SetVisibility(ESlateVisibility::Hidden);
	} else {UE_LOG(LogTemp, Error, TEXT("Failed to get the Player Controller Reference"));}
}

void USkillTreeComponent::EquipSkill(AStudyPC* Controller)
{
	
}



