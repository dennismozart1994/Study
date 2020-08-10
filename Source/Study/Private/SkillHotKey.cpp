// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillHotKey.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "SkillTreeComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "StudyCharacter.h"
#include "StudyPC.h"
#include "MasterSkill.h"

bool USkillHotKey::Initialize()
{
    bool success = Super::Initialize();
    if(!success) return false;

    if(!ensure(SkillButton != nullptr) && !ensure(SkillIcon != nullptr)) return false;
    SkillButton->OnClicked.AddDynamic(this, &USkillHotKey::OnSlotClicked);
    return success;
}

void USkillHotKey::NativeConstruct()
{
    Super::NativeConstruct();
    bCanCastSkill = true;
    CoolDownMaterial = CoolDownImage->GetDynamicMaterial();
}

void USkillHotKey::OnSlotClicked()
{
    // If user implements Skill Tree Component, than interact with the UI
    if(this->GetOwningPlayerPawn())
    {
        USkillTreeComponent* Component = Cast<USkillTreeComponent>(
            this->GetOwningPlayerPawn()->FindComponentByClass(USkillTreeComponent::StaticClass()));
        if(Component)
        {
            AStudyCharacter* PlayerRef = Cast<AStudyCharacter>(GetOwningPlayerPawn());
            if(PlayerRef)
            {
                if(Component->bIsEquippingSkill)
                {
                    Component->EquipSkill(SlotIndex);
                    Component->StopSkillBarHighlight();
                    UE_LOG(LogTemp, Log, TEXT("Skill Has been equipped"))
                } else if(PlayerRef->CurrentSkillCast[SlotIndex])
                {
                    if(bCanCastSkill)
                    {
                        if(PlayerRef->CurrentSkillCast[SlotIndex]->SkillDetails.MontageToPlay)
                        {
                            PlayerRef->Multicast_PlayMontage(PlayerRef->CurrentSkillCast[SlotIndex]->SkillDetails.MontageToPlay);
                            PlayerRef->CurrentSkillCast[SlotIndex]->CoolDown();
                            UE_LOG(LogTemp, Log, TEXT("Casting Skill"))
                            bCanCastSkill = false;
                        } else
                        {
                            UE_LOG(LogTemp, Error, TEXT("Invalid Montage to Skill Ref"))
                        }
                    } else
                    {
                        UE_LOG(LogTemp, Warning, TEXT("Skill is Cooling Down"))
                    }
                } else
                {
                    UE_LOG(LogTemp, Warning, TEXT("Invalid Skill Reference, maybe the slot is empty?"))
                }
            } else
            {
                UE_LOG(LogTemp, Error, TEXT("Failed to Cast to Player Character"))
            }
        }
    }
}

void USkillHotKey::SetCoolDownText(FText content)
{
    CoolDownValueInSeconds = content;
}

void USkillHotKey::SetCoolDownTextVisibility(ESlateVisibility desired)
{
    CoolDownText->SetVisibility(desired);
}

void USkillHotKey::SetCoolDownImageVisibility(ESlateVisibility desired)
{
    CoolDownImage->SetVisibility(desired);
}

void USkillHotKey::SetSkillIconColour(FLinearColor colour)
{
    SkillIcon->SetColorAndOpacity(colour);
}

void USkillHotKey::SpawnSkill()
{
    AStudyCharacter* StudyCharacterRef = Cast<AStudyCharacter>(GetOwningPlayer()->GetPawn());
    if(UKismetSystemLibrary::IsValidClass(SkillActor) && StudyCharacterRef)
    {
        StudyCharacterRef->SpawnSkill(SkillActor, SlotIndex);
        if(StudyCharacterRef->CurrentSkillCast[SlotIndex])
        {
            StudyCharacterRef->CurrentSkillCast[SlotIndex]->SkillSlotRef = this;
            UE_LOG(LogTemp, Log, TEXT("Successfully Spawned Skill and assign it to it's slot reference"))
        }
    }
}