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
                } else if(SkillIcon != NULL)
                {
                    if(bCanCastSkill && UKismetSystemLibrary::IsValidClass(SkillActor))
                    {
                        if(PlayerRef->isMovingOnGroundCheck())
                        {
                            AStudyPC* PCRef = Cast<AStudyPC>(PlayerRef->Controller);
                            if(PCRef)
                            {
                                if(PCRef->GetRemoteRole() == ROLE_Authority)
                                {
                                    PCRef->Server_CastSkill(PCRef, SlotIndex, SkillActor);
                                    UE_LOG(LogTemp, Log, TEXT("Server Called Cast Skill"))
                                } else
                                {
                                    UE_LOG(LogTemp, Log, TEXT("Client Called Cast Skill"))
                                    PCRef->Client_CastSkill(PCRef, SlotIndex, SkillActor);
                                }
                            } else
                            {
                                UE_LOG(LogTemp, Warning, TEXT("Invalid Player Controller"))
                            }
                        } else
                        {
                            UE_LOG(LogTemp, Warning, TEXT("Player in on Air"))
                        }
                    } else
                    {
                        UE_LOG(LogTemp, Warning, TEXT("Skill is Cooling Down or skill class is invalid"))
                    }
                } else
                {
                    UE_LOG(LogTemp, Warning, TEXT("Invalid Skill Icon, maybe the slot is empty?"))
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