// Fill out your copyright notice in the Description page of Project Settings.

#include "StudyPC.h"
#include "Net/UnrealNetwork.h"
#include "StudyPlayerState.h"
#include "StudyCharacter.h"
#include "Engine/DataTable.h"
#include "Animation/AnimMontage.h"
#include "MasterSkill.h"
#include "SkillTreeComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

AStudyPC::AStudyPC()
{
    bReplicates = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
	bEnableTouchEvents = false;
	ClickEventKeys.Add(EKeys::LeftMouseButton);
	ClickEventKeys.Add(EKeys::RightMouseButton);
	DefaultClickTraceChannel = ECollisionChannel::ECC_Camera;

	// Initialize Equipped Skills with null values
	for(int i=1; i<=6; i++)
	{
		WarriorSkills.Add(FName("None"));
		ArcherSkills.Add(FName("None"));
		MageSkills.Add(FName("Name"));
	}
	
}

void AStudyPC::Server_UnlockSkill_Implementation(AStudyPC* Controller, FName Skill)
{
	if(Controller) {
		AStudyPlayerState* State = Cast<AStudyPlayerState>(Controller->PlayerState);
		if(State) {
			// If is the server who's calling the unlock and the user hasn't already unlocked the Skill
			if((State->GetLocalRole() == ROLE_Authority || (State->GetLocalRole() == ROLE_Authority && State->GetRemoteRole() < ROLE_AutonomousProxy)) && !Controller->CharacterSkills.Contains(Skill)
					&& !Skill.ToString().IsEmpty()) {
				State->CharacterStats.GoldAmount-=getSkillDetails(Skill).PriceToUnlock;
				Controller->CharacterSkills.Add(Skill);
			} else {Client_UnlockSkill(Controller, Skill);}
		} else {UE_LOG(LogTemp, Error, TEXT("Error trying to cast to Player State"));}
	} else {UE_LOG(LogTemp, Error, TEXT("Invalid Controller passed as a reference to the function"));}
}

bool AStudyPC::Server_UnlockSkill_Validate(AStudyPC* Controller, FName Skill)
{
	return true;
}

void AStudyPC::Client_UnlockSkill_Implementation(AStudyPC *Controller, FName Skill)
{
	Server_UnlockSkill(Controller, Skill);
}

void AStudyPC::Server_EquipSkill_Implementation(AStudyPC* Controller, FName Skill, int32 SlotIndex)
{
	const FSkilDataTable SkillDetails = getSkillDetails(Skill);
	switch (SkillDetails.TreeClass)
	{
		case ESkillClass::SC_Warrior: WarriorSkills[SlotIndex] = Skill;
		break;
		case ESkillClass::SC_Archier: ArcherSkills[SlotIndex] = Skill;
		break;
		case ESkillClass::SC_Magician: MageSkills[SlotIndex] = Skill;
		break;
		default: UE_LOG(LogTemp, Error, TEXT("Invalid Skill Class"));
		break;
	}
}

bool AStudyPC::Server_EquipSkill_Validate(AStudyPC* Controller, FName Skill, int32 SlotIndex)
{
	return true;
}

void AStudyPC::Client_EquipSkill_Implementation(AStudyPC* Controller, FName Skill, int32 SlotIndex)
{
	Server_EquipSkill_Implementation(Controller, Skill, SlotIndex);
}

TArray<FName> AStudyPC::GetSkillArray(ESkillClass Tree) const
{
	TArray<FName> Empty;
	switch (Tree)
	{
		case ESkillClass::SC_Warrior:
			UE_LOG(LogTemp, Log, TEXT("Grabbed Warrior Skills"));
			return WarriorSkills;
		case ESkillClass::SC_Archier:
			UE_LOG(LogTemp, Log, TEXT("Grabbed Archer Skills"));
			return ArcherSkills;
		case ESkillClass::SC_Magician:
			UE_LOG(LogTemp, Log, TEXT("Grabbed Mage Skills"));
			return MageSkills;
		default:
			UE_LOG(LogTemp, Error, TEXT("Returned Empty Skills Array"));
			return Empty;
	}
}

void AStudyPC::Server_CastSkill_Implementation(AStudyPC* Controller, int32 SkillIndex, TSubclassOf<AMasterSkill> SkillClass)
{
	if(GetLocalRole() == ROLE_Authority || (GetLocalRole() == ROLE_Authority && GetRemoteRole() < ROLE_AutonomousProxy))
	{
		AStudyCharacter* PlayerRef = Cast<AStudyCharacter>(Controller->GetPawn());
		if(PlayerRef)
		{
			UWorld* World = GetWorld();

			AStudyPlayerState* PSRef = Cast<AStudyPlayerState>(PlayerState);
			if(PSRef)
			{
				if(UKismetSystemLibrary::IsValidClass(SkillClass) && World && PSRef->GetPawn())
				{
					FActorSpawnParameters SpawnParams;
					SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
					SpawnParams.Owner = PSRef->GetPawn();
					FVector Location = PSRef->GetPawn()->GetActorLocation() + PSRef->GetPawn()->GetActorForwardVector() * 100;
					FRotator Rotation = PSRef->GetPawn()->GetActorRotation();
					FTransform LocationToSpawn = UKismetMathLibrary::MakeTransform(Location, Rotation, FVector(1.f));
					if(PlayerRef->CurrentSkillCast[SkillIndex]) PlayerRef->CurrentSkillCast[SkillIndex]->SetLifeSpan(5.f);
					AMasterSkill* SkillRef = World->SpawnActor<AMasterSkill>(SkillClass, LocationToSpawn, SpawnParams);
					if(SkillRef)
					{
						SkillRef->SkillSlotIndex = SkillIndex;
						PlayerRef->CurrentSkillCast[SkillIndex] = SkillRef;
						UE_LOG(LogTemp, Log, TEXT("Successfully Spawned Skill and assign it to it's slot reference"))
					} else {
						UE_LOG(LogTemp, Error, TEXT("Error trying to store a valid Skill Actor to the current Array index"))
					}
				
					PlayerRef->SkillMontage = PlayerRef->CurrentSkillCast[SkillIndex]->SkillDetails.MontageToPlay;
					PlayerRef->Multicast_PlayMontage(PlayerRef->SkillMontage);
					// Need to find a way of passing the UI Ref in here (Maybe the Index?)
					UE_LOG(LogTemp, Log, TEXT("CoolDown on Slot %s"), *FString::FromInt(SkillIndex));
					PlayerRef->CurrentSkillCast[SkillIndex]->CoolDown();
					UE_LOG(LogTemp, Log, TEXT("Casting Skill"))
					PlayerRef->GetSkillTreeComponent()->bCanCastSkill = false;
					PlayerRef->bCanWalk = false;
				} else
				{
					UE_LOG(LogTemp, Warning, TEXT("Error trying to Spawn Skill"))
				}
			} else
			{
				UE_LOG(LogTemp, Warning, TEXT("Invalid Player State"))
			}
		}else
		{
			UE_LOG(LogTemp, Error, TEXT("Invalid Player Ref"))
		}
	} else
	{
		Client_CastSkill(Controller, SkillIndex, SkillClass);
	}
}

bool AStudyPC::Server_CastSkill_Validate(AStudyPC* Controller, int32 SkillIndex, TSubclassOf<AMasterSkill> SkillClass)
{
	return true;
}

void AStudyPC::Client_CastSkill_Implementation(AStudyPC* Controller, int32 SkillIndex,  TSubclassOf<AMasterSkill> SkillClass)
{
	Server_CastSkill(Controller, SkillIndex, SkillClass);
}


FSkilDataTable AStudyPC::getSkillDetails(FName rowName)
{
	// load data table
	static const FString ContextCurrent(TEXT("Current Item Details"));
	FSkilDataTable* row = SkillTable->FindRow<FSkilDataTable>(rowName, ContextCurrent, true);
	if(row) return *(row);
	return FSkilDataTable();
}

AStudyPlayerState* AStudyPC::GetPersonalPlayerState()
{
    AStudyPlayerState* MyPlayerState = Cast<AStudyPlayerState>(PlayerState);
    return MyPlayerState;
}

void AStudyPC::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
 {
     Super::GetLifetimeReplicatedProps(OutLifetimeProps);
 
     DOREPLIFETIME(AStudyPC, Inventory);
	 DOREPLIFETIME(AStudyPC, InventoryItems);
	 DOREPLIFETIME(AStudyPC, CharacterSkills);
	 DOREPLIFETIME(AStudyPC, WarriorSkills);
	 DOREPLIFETIME(AStudyPC, ArcherSkills);
	 DOREPLIFETIME(AStudyPC, MageSkills);
}
