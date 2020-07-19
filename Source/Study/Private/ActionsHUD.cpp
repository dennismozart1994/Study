// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionsHUD.h"
#include "Components/Button.h"

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

    if(CurrentTab == ESkillClass::SC_Warrior) SetTabAsActive(WarriorButton);
    if(CurrentTab == ESkillClass::SC_Archier) SetTabAsActive(ArcherButton);
    if(CurrentTab == ESkillClass::SC_Magician) SetTabAsActive(MageButton);
    
    WarriorButton->SetVisibility(ESlateVisibility::Hidden);
    ArcherButton->SetVisibility(ESlateVisibility::Hidden);
    MageButton->SetVisibility(ESlateVisibility::Hidden);

    for(ESkillClass Tab: TabsAvailable)
    {
        if(Tab == ESkillClass::SC_Warrior) WarriorButton->SetVisibility(ESlateVisibility::Visible);

        if(Tab == ESkillClass::SC_Archier) ArcherButton->SetVisibility(ESlateVisibility::Visible);

        if(Tab == ESkillClass::SC_Magician) MageButton->SetVisibility(ESlateVisibility::Visible);
    }
}

void UActionsHUD::OnWarriorClicked()
{
    SetTabAsActive(WarriorButton);
    SetTabAsDeactivated(ArcherButton);
    SetTabAsDeactivated(MageButton);
}

void UActionsHUD::OnArcherClicked()
{
    SetTabAsDeactivated(WarriorButton);
    SetTabAsActive(ArcherButton);
    SetTabAsDeactivated(MageButton);
}

void UActionsHUD::OnMageClicked()
{
    SetTabAsDeactivated(WarriorButton);
    SetTabAsDeactivated(ArcherButton);
    SetTabAsActive(MageButton);
}

void UActionsHUD::SetTabAsActive(UButton* Button)
{
    Button->WidgetStyle.Normal.TintColor = ActiveColour;
}

void UActionsHUD::SetTabAsDeactivated(UButton* Button)
{
    Button->WidgetStyle.Normal.TintColor = NormalColour;
}




