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
    CoolDownMaterial = CoolDownImage->GetDynamicMaterial();
    UWorld* World = GetOwningPlayer()->GetWorld();
    AStudyCharacter* StudyCharacterRef = Cast<AStudyCharacter>(GetOwningPlayer()->GetPawn());
    if(!UKismetSystemLibrary::IsValid(SkillRef) && UKismetSystemLibrary::IsValidClass(SkillActor)
        && World && StudyCharacterRef) {
        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        FVector Location = StudyCharacterRef->GetActorLocation() + StudyCharacterRef->GetActorForwardVector() * 100;
        FRotator Rotation = StudyCharacterRef->GetActorRotation();
        FTransform LocationToSpawn = UKismetMathLibrary::MakeTransform(Location, Rotation, FVector(1.f));
        // @TODO Take care of replication later, the skill should be spawned and only allowed in the server
        // @TODO Clients should not be allowed to spawn any skill, otherwise it won't do anything on a Multiplayer environment
        // @TODO Put this spawn into somewhere else later (maybe into the character itself so it can be set as it's owner
        SkillRef = World->SpawnActor<AMasterSkill>(SkillActor, LocationToSpawn, SpawnParams);
        if(SkillRef)
        {
            SkillRef->SkillSlotRef = this;
            UE_LOG(LogTemp, Log, TEXT("Successfully Spawned Skill and assign it to it's slot reference"))
        }
        
    }
}

void USkillHotKey::OnSlotClicked()
{
    // If user implements Skill Tree Component, than interact with the UI
    if(this->GetOwningPlayerPawn() && SkillRef)
    {
        USkillTreeComponent* Component = Cast<USkillTreeComponent>(
            this->GetOwningPlayerPawn()->FindComponentByClass(USkillTreeComponent::StaticClass()));
        if(Component)
        {
            if(Component->bIsEquippingSkill)
            {
                Component->EquipSkill(SlotIndex);
                Component->StopSkillBarHighlight();
            } else
            {
                // @TODO Spawn Skill and blocked until the cool down is done
                SkillRef->CoolDown();
                CoolDownTimeline();
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




