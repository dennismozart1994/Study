// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillHotKey.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "StudyCharacter.h"

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
    UWorld* World = GetWorld();
    AStudyCharacter* StudyCharacterRef = Cast<AStudyCharacter>(GetOwningPlayer()->GetPawn());
    if(!UKismetSystemLibrary::IsValid(CoolDownRef) && UKismetSystemLibrary::IsValidClass(CoolDownActor)
        && World && StudyCharacterRef) {
        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        FVector Location = StudyCharacterRef->GetActorLocation() + StudyCharacterRef->GetActorForwardVector() * 100;
        FRotator Rotation = StudyCharacterRef->GetActorRotation();
        FTransform LocationToSpawn = UKismetMathLibrary::MakeTransform(Location, Rotation, FVector(1.f));
        CoolDownRef = World->SpawnActor<AActor>(CoolDownActor, LocationToSpawn, SpawnParams);
    }
}

void USkillHotKey::OnSlotClicked()
{
    CoolDownTimeline();
}
