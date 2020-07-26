// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionsHUD.h"
#include "Components/Button.h"
#include "Components/Border.h"
#include "SkillTreeComponent.h"
#include "SkillHotKey.h"
#include "Animation/WidgetAnimation.h"

bool UActionsHUD::Initialize()
{
    bool success = Super::Initialize();
    if(!success) return false;

    if(!ensure(WarriorButton != nullptr) && !ensure(ArcherButton != nullptr) && !ensure(MageButton != nullptr)) return false;
    WarriorButton->OnClicked.AddDynamic(this, &UActionsHUD::OnWarriorClicked);
    ArcherButton->OnClicked.AddDynamic(this, &UActionsHUD::OnArcherClicked);
    MageButton->OnClicked.AddDynamic(this, &UActionsHUD::OnMageClicked);
    return success;
}

void UActionsHUD::NativeConstruct()
{
    Super::NativeConstruct();

    SetTabAsActive(CurrentTab);
    
    WarriorButton->SetVisibility(ESlateVisibility::Hidden);
    ArcherButton->SetVisibility(ESlateVisibility::Hidden);
    MageButton->SetVisibility(ESlateVisibility::Hidden);

    UpdateButtonsVisibility();
}

void UActionsHUD::OnWarriorClicked()
{
    SetTabAsActive(ESkillClass::SC_Warrior);
    SetTabAsDeactivated(ESkillClass::SC_Archier);
    SetTabAsDeactivated(ESkillClass::SC_Magician);
    
    // Disable EquipMode If needed
    USkillTreeComponent* Component = Cast<USkillTreeComponent>(
    this->GetOwningPlayerPawn()->FindComponentByClass(USkillTreeComponent::StaticClass()));
    if(Component)
    {
        Component->StopSkillBarHighlight();
        Component->UpdateSkillSlots(ESkillClass::SC_Warrior);
    }
}

void UActionsHUD::OnArcherClicked()
{
    SetTabAsDeactivated(ESkillClass::SC_Warrior);
    SetTabAsActive(ESkillClass::SC_Archier);
    SetTabAsDeactivated(ESkillClass::SC_Magician);

    // Disable EquipMode If needed
    USkillTreeComponent* Component = Cast<USkillTreeComponent>(
    this->GetOwningPlayerPawn()->FindComponentByClass(USkillTreeComponent::StaticClass()));
    if(Component)
    {
        Component->StopSkillBarHighlight();
        Component->UpdateSkillSlots(ESkillClass::SC_Archier);
    }
}

void UActionsHUD::OnMageClicked()
{
    SetTabAsDeactivated(ESkillClass::SC_Warrior);
    SetTabAsDeactivated(ESkillClass::SC_Archier);
    SetTabAsActive(ESkillClass::SC_Magician);

    // Disable EquipMode If needed
    USkillTreeComponent* Component = Cast<USkillTreeComponent>(
    this->GetOwningPlayerPawn()->FindComponentByClass(USkillTreeComponent::StaticClass()));
    if(Component)
    {
        Component->StopSkillBarHighlight();
        Component->UpdateSkillSlots(ESkillClass::SC_Magician);
    }
}

void UActionsHUD::UpdateButtonsVisibility()
{
    for(ESkillClass Tab: TabsAvailable)
    {
        if(Tab == ESkillClass::SC_Warrior) WarriorButton->SetVisibility(ESlateVisibility::Visible);

        if(Tab == ESkillClass::SC_Archier) ArcherButton->SetVisibility(ESlateVisibility::Visible);

        if(Tab == ESkillClass::SC_Magician) MageButton->SetVisibility(ESlateVisibility::Visible);
    }
}


void UActionsHUD::SetTabAsActive(ESkillClass Tab)
{
    if(Tab == ESkillClass::SC_Warrior) WarriorButton->WidgetStyle.Normal.TintColor = ActiveColour;
    if(Tab == ESkillClass::SC_Archier) ArcherButton->WidgetStyle.Normal.TintColor = ActiveColour;
    if(Tab == ESkillClass::SC_Magician) MageButton->WidgetStyle.Normal.TintColor = ActiveColour;
    CurrentTab = Tab;
}

void UActionsHUD::SetTabAsDeactivated(ESkillClass Tab)
{
    if(Tab == ESkillClass::SC_Warrior) WarriorButton->WidgetStyle.Normal.TintColor = NormalColour;
    if(Tab == ESkillClass::SC_Archier) ArcherButton->WidgetStyle.Normal.TintColor = NormalColour;
    if(Tab == ESkillClass::SC_Magician) MageButton->WidgetStyle.Normal.TintColor = NormalColour;
}

void UActionsHUD::StartAnimation()
{
    PlayHighlightAnimation();
}

void UActionsHUD::CancelAnimation()
{
    StopHighlightAnimation();
}






