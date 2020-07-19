// Fill out your copyright notice in the Description page of Project Settings.


#include "PickupInfo.h"

#include "InventoryComponent.h"
#include "Components/Button.h"
#include "InventoryComponent.h"

bool UPickupInfo::Initialize()
{
    bool success = Super::Initialize();
    if(!success) return false;

    if(!ensure(PreviewButton != nullptr) && !ensure(CollectButton != nullptr) && !ensure(CancelButton != nullptr)) return false;
    CollectButton->OnClicked.AddDynamic(this, &UPickupInfo::OnCollectClick);
    PreviewButton->OnClicked.AddDynamic(this, &UPickupInfo::OnPreviewClick);
    CancelButton->OnClicked.AddDynamic(this, &UPickupInfo::OnCancelClick);
    return success;
}

void UPickupInfo::NativeConstruct()
{
    Super::NativeConstruct();
}

void UPickupInfo::OnCollectClick()
{
    if(this->GetOwningPlayerPawn())
    {
        UInventoryComponent* Component = Cast<UInventoryComponent>(
            this->GetOwningPlayerPawn()->FindComponentByClass(UInventoryComponent::StaticClass()));
        if(Component)
        {
            Component->PickupItem(PickupReference);
            RemoveFromParent();
        }
    }
}

void UPickupInfo::OnPreviewClick()
{
    if(this->GetOwningPlayerPawn())
    {
        UInventoryComponent* Component = Cast<UInventoryComponent>(
            this->GetOwningPlayerPawn()->FindComponentByClass(UInventoryComponent::StaticClass()));
        if(Component) Component->PresentItem3D();
    }
        
}


void UPickupInfo::OnCancelClick()
{
    RemoveFromParent();
}



