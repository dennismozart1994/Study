// Fill out your copyright notice in the Description page of Project Settings.

#include "AnimInstanceDefault.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "SkillTreeComponent.h"
#include "Runtime/Engine/Classes/Components/CapsuleComponent.h"

UAnimInstanceDefault::UAnimInstanceDefault()
{
	bDoOnce = true;
	bIsAlive = true;
	WeaponBeingUsed = EWeaponType::WT_None;
}

void UAnimInstanceDefault::UpdateAnimProperties()
{
	AStudyCharacter* Pawn = Cast<AStudyCharacter>(TryGetPawnOwner());
	
	if (Pawn) {
		CharacterSpeed = Pawn->GetVelocity().Size();
		PlayerStateRef = Cast<AStudyPlayerState>(Pawn->GetPlayerState());
		WeaponBeingUsed = Pawn->WeaponBeingUsed;
		bIsCastingSkill = !Pawn->GetSkillTreeComponent()->bCanCastSkill;
		if(PlayerStateRef)
		{
			bIsAlive = PlayerStateRef->bIsAlive;
		}
	}
}

// Reset status so the player can attack again
void UAnimInstanceDefault::ResetCanAttack()
{
	AStudyCharacter* PlayerRef = Cast<AStudyCharacter>(TryGetPawnOwner());
	if (PlayerRef)
	{
		PlayerRef->bIsReceivingDamage = false;
		PlayerRef->bCanAttack = true;
		FRotator CurrentRotation = PlayerRef->GetCapsuleComponent()->GetRelativeRotation();
		PlayerRef->GetCapsuleComponent()->SetRelativeRotation(FRotator(0.f, CurrentRotation.Yaw, 0.f));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Error trying to get player reference during cast on function ResetCanAttack() at the Anim Instance"));
	}
}

void UAnimInstanceDefault::ResetCanWalk()
{
	AStudyCharacter* PlayerRef = Cast<AStudyCharacter>(TryGetPawnOwner());
	if (PlayerRef)
	{
		PlayerRef->GetSkillTreeComponent()->bCanCastSkill = true;
		PlayerRef->bCanWalk = true;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Error trying to get player reference during cast on function ResetCanAttack() at the Anim Instance"));
	}
}


void UAnimInstanceDefault::DoOnce()
{
	if(bDoOnce)
	{
		DeathIndex = UKismetMathLibrary::RandomIntegerInRange(0, 4);
	}
}

void UAnimInstanceDefault::ResetDoOnce()
{
	bDoOnce = true;
}
