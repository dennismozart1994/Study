// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillTreeComponent.h"
#include "StudyCharacter.h"
#include "StudyPC.h"
#include "MasterSkill.h"
#include "Skill3DPreview.h"
#include "Blueprint/UserWidget.h"
#include "GameplayHUD.h"
#include "ActionsHUD.h"
#include "SkillDescription.h"
#include "Skill_Slot_Defaults.h"
#include "SkillHotKey.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"

// Sets default values for this component's properties
USkillTreeComponent::USkillTreeComponent()
{
	bIsEquippingSkill = false;
	bCanCastSkill = true;
	MaxSlots = 6;
	SkillHotKeys.Add(EKeys::One);
	SkillHotKeys.Add(EKeys::Two);
	SkillHotKeys.Add(EKeys::Three);
	SkillHotKeys.Add(EKeys::Four);
	SkillHotKeys.Add(EKeys::Five);
	SkillHotKeys.Add(EKeys::Six);
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

void USkillTreeComponent::SkillBarHighlight()
{
	AStudyCharacter* PlayerRef = Cast<AStudyCharacter>(GetOwner());
	if(PlayerRef)
	{
		UGameplayHUD* gmpHUD = PlayerRef->HudRef;
		if(gmpHUD)
		{
			UActionsHUD* Actions = gmpHUD->Actions;
			if(Actions)
			{
				// Just in case deactivate all other tabs
				Actions->SetTabAsDeactivated(ESkillClass::SC_Warrior);
				Actions->SetTabAsDeactivated(ESkillClass::SC_Archier);
				Actions->SetTabAsDeactivated(ESkillClass::SC_Magician);

				// And only activates back the right one
				Actions->SetTabAsActive(SkillToEquip.TreeClass);
				Actions->StartAnimation();
				bIsEquippingSkill = true;
			}
		}
	}
}

void USkillTreeComponent::StopSkillBarHighlight()
{
	AStudyCharacter* PlayerRef = Cast<AStudyCharacter>(GetOwner());
	if(PlayerRef)
	{
		UGameplayHUD* gmpHUD = PlayerRef->HudRef;
		if(gmpHUD)
		{
			UActionsHUD* Actions = gmpHUD->Actions;
			if(Actions)
			{
				Actions->StopHighlightAnimation();
				bIsEquippingSkill = false;
			}
		}
	}
}

void USkillTreeComponent::StartEquipSkill()
{
	if(SkillDetailsRef)
	{
		if(SkillDetailsRef->SkillSlotRef)
		{
			// Store the Skill into a local variable to use later once we click on the actual slot
			CurrentSkillRow = SkillDetailsRef->SkillSlotRef->SkillRow;
			SkillToEquip = GetSkillTreeItem(CurrentSkillRow);
			// Highlight Effect on the Border around the Skill Slots
			SkillBarHighlight();
		}
	}
}

void USkillTreeComponent::EquipSkill(int32 SlotIndex)
{
	AStudyCharacter* PlayerRef = Cast<AStudyCharacter>(GetOwner());
	if(PlayerRef)
	{
		AStudyPC* ControllerRef = Cast<AStudyPC>(PlayerRef->GetController());
		if(ControllerRef)
		{
			if(GetOwnerRole() == ROLE_Authority)
			{
				ControllerRef->Server_EquipSkill(ControllerRef, CurrentSkillRow, SlotIndex);
			}
			else
			{
				ControllerRef->Client_EquipSkill(ControllerRef, CurrentSkillRow, SlotIndex);
			}
			StopSkillBarHighlight();
			UpdateSkillSlots(SkillToEquip.TreeClass);
		}
	}
}

void USkillTreeComponent::UpdateSkillSlots(ESkillClass Tree)
{
	AStudyCharacter* PlayerRef = Cast<AStudyCharacter>(GetOwner());
	if(PlayerRef)
	{
		AStudyPC* PCRef = Cast<AStudyPC>(PlayerRef->GetController());
		if(PCRef)
		{
			TArray<FName> Skills = PCRef->GetSkillArray(Tree);
			TArray<USkillHotKey*> Slots;
			UGameplayHUD* gmpHUD = PlayerRef->HudRef;
			if(gmpHUD)
			{
				UActionsHUD* Actions = gmpHUD->Actions;
				if(Actions)
				{
					// Save Skill Slots References
					Slots.Add(Actions->SkillSlot0);
					Slots.Add(Actions->SkillSlot1);
					Slots.Add(Actions->SkillSlot2);
					Slots.Add(Actions->SkillSlot3);
					Slots.Add(Actions->SkillSlot4);
					Slots.Add(Actions->SkillSlot5);
					for(int32 i = 0; i < Skills.Num(); i++)
					{
						// If there's a Skill on the current index
						if(Skills[i] != FName("None"))
						{
							// Grab the right Skill Type Array
							FSkilDataTable SkillTreeDetail = GetSkillTreeItem(Skills[i]);
							// If there's a valid Skill Class on that row, than grab its values
							if(UKismetSystemLibrary::IsValidClass(SkillTreeDetail.SkillClass)) {
								Slots[i]->SkillActor = SkillTreeDetail.SkillClass;
								Slots[i]->SpawnSkill();
								Slots[i]->HotKey = SkillHotKeys[i];
								AMasterSkill* DefaultActor = Cast<AMasterSkill>(SkillTreeDetail.SkillClass->GetDefaultObject(true));
								if(DefaultActor)
								{
									// If there's a thumbnail set that and make it visible
									// in any other case, hide the thumbnail because either some value is null or None
									if(DefaultActor->SkillDetails.SkillThumbnail && Slots[i])
									{
										Slots[i]->SkillIcon->SetBrushFromTexture(DefaultActor->SkillDetails.SkillThumbnail);
										Slots[i]->SkillIcon->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
										UE_LOG(LogTemp, Log, TEXT("Update Slots"));
									} else
									{
										Slots[i]->SkillIcon->SetVisibility(ESlateVisibility::Hidden);
										UE_LOG(LogTemp, Error, TEXT("Thumbnail or Slots are null"));
									}
								}else
								{
									Slots[i]->SkillIcon->SetVisibility(ESlateVisibility::Hidden);
									UE_LOG(LogTemp, Error, TEXT("Failed to cast to Master Skill Class"));
								}
							}else
							{
								Slots[i]->SkillIcon->SetVisibility(ESlateVisibility::Hidden);
								UE_LOG(LogTemp, Error, TEXT("Skill Class is invalid"));
							}
						} else
						{
							Slots[i]->SkillIcon->SetVisibility(ESlateVisibility::Hidden);
							UE_LOG(LogTemp, Log, TEXT("Skill is None"));
						}
					}
				}else
				{
					UE_LOG(LogTemp, Error, TEXT("Actions HUD Reference is null"));
				}
			}else
			{
				UE_LOG(LogTemp, Error, TEXT("Gameplay HUD is null"));
			}
		}else
		{
			UE_LOG(LogTemp, Error, TEXT("Controller reference is null"));
		}
	}else
	{
		UE_LOG(LogTemp, Error, TEXT("Player Character Reference is null"));
	}
}

void USkillTreeComponent::CastSkill(FKey KeyPressed)
{
	if(SkillHotKeys.Contains(KeyPressed) && SkillHotKeys.Num() <= MaxSlots)
	{
		AStudyCharacter* PlayerRef = Cast<AStudyCharacter>(GetOwner());
		if(PlayerRef)
		{
			AStudyPC* PCRef = Cast<AStudyPC>(PlayerRef->GetController());
			if(PCRef)
			{
				UGameplayHUD* gmpHUD = PlayerRef->HudRef;
				if(gmpHUD)
				{
					UActionsHUD* Actions = gmpHUD->Actions;
					if(Actions)
					{
						if(SkillHotKeys[0] == KeyPressed) Actions->SkillSlot0->OnSlotClicked();
						if(SkillHotKeys[1] == KeyPressed) Actions->SkillSlot1->OnSlotClicked();
						if(SkillHotKeys[2] == KeyPressed) Actions->SkillSlot2->OnSlotClicked();
						if(SkillHotKeys[3] == KeyPressed) Actions->SkillSlot3->OnSlotClicked();
						if(SkillHotKeys[4] == KeyPressed) Actions->SkillSlot4->OnSlotClicked();
						if(SkillHotKeys[5] == KeyPressed) Actions->SkillSlot5->OnSlotClicked();
					}
				}
			}
		}
	}
}






