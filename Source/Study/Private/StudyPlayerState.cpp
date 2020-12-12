// Fill out your copyright notice in the Description page of Project Settings.

#include "StudyPlayerState.h"
#include "Engine.h"
#include "StudyCharacter.h"
#include "Engine/DataTable.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

AStudyPlayerState::AStudyPlayerState()
{
    // Gameplay Variables
	 CharacterStats.CharacterName =  "TEST CHARACTER";
	 CharacterStats.Strenght = 30;
	 CharacterStats.Speed = 400;
	 CharacterStats.ActualLife = 100;
	 CharacterStats.FullLife = 100;
	 CharacterStats.ActualMana = 1000;
	 CharacterStats.FullMana = 1000;
	 CharacterStats.ActualStamina = 100;
	 CharacterStats.FullStamina = 100;
	 CharacterStats.CurrentLevel = 1;
	 CharacterStats.GoldAmount = 1;
	 CharacterStats.GoNextLevelWhen = 100;
	 bIsAlive = true;
}

void AStudyPlayerState::updateCharStats(UDataTable* TableRef, FName Actual, FName New)
{
	static const FString ContextCurrent(TEXT("Current Armor Set Details"));
	FItemDetailsDataTable* CurrentArmor = TableRef->FindRow<FItemDetailsDataTable>(Actual, ContextCurrent, true);

	static const FString ContextNew(TEXT("New Armor Set Details"));
	FItemDetailsDataTable* NewArmorSet = TableRef->FindRow<FItemDetailsDataTable>(New, ContextNew, true);
	
	if(TableRef)
	{
		// swap between armorsets
		if(Actual != "None" && New != "None")
		{
			if(CurrentArmor && NewArmorSet)
			{
				CharacterStats.ActualLife += NewArmorSet->Life - CurrentArmor->Life;
				CharacterStats.FullLife += NewArmorSet->Life - CurrentArmor->Life;

				CharacterStats.ActualMana += NewArmorSet->MagicPoints - CurrentArmor->MagicPoints;
				CharacterStats.FullMana += NewArmorSet->MagicPoints - CurrentArmor->MagicPoints;

				CharacterStats.ActualStamina += NewArmorSet->Stamina - CurrentArmor->Stamina;
				CharacterStats.FullStamina += NewArmorSet->Stamina - CurrentArmor->Stamina;

				CharacterStats.Speed += NewArmorSet->Speed - CurrentArmor->Speed;
				AStudyCharacter* CharacterRef = Cast<AStudyCharacter>(this->GetPawn());
				if(CharacterRef)
				{
					CharacterRef->GetCharacterMovement()->MaxWalkSpeed = float(CharacterStats.Speed);
				}

				CharacterStats.Strenght += NewArmorSet->Strenght - CurrentArmor->Strenght;
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Error trying to find items on data table");
			}
		}
		// Add new Item to Armor Set
		else if(Actual == "None" && New != "None")
		{
			if(NewArmorSet)
			{
				CharacterStats.ActualLife += NewArmorSet->Life;
				CharacterStats.FullLife += NewArmorSet->Life;

				CharacterStats.ActualMana += NewArmorSet->MagicPoints;
				CharacterStats.FullMana += NewArmorSet->MagicPoints;

				CharacterStats.ActualStamina += NewArmorSet->Stamina;
				CharacterStats.FullStamina += NewArmorSet->Stamina;

				CharacterStats.Speed += NewArmorSet->Speed;
				AStudyCharacter* CharacterRef = Cast<AStudyCharacter>(this->GetPawn());
				if(CharacterRef)
				{
					CharacterRef->GetCharacterMovement()->MaxWalkSpeed = float(CharacterStats.Speed);
				}

				CharacterStats.Strenght += NewArmorSet->Strenght;
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Error trying to find new item on data table");
			}
		}
		// Remove Item from Armor Set
		else if(Actual != "None" && New == "None")
		{
			if(CurrentArmor)
			{
				CharacterStats.ActualLife -= CurrentArmor->Life;
				CharacterStats.FullLife -= CurrentArmor->Life;

				CharacterStats.ActualMana -= CurrentArmor->MagicPoints;
				CharacterStats.FullMana -= CurrentArmor->MagicPoints;

				CharacterStats.ActualStamina -= CurrentArmor->Stamina;
				CharacterStats.FullStamina -= CurrentArmor->Stamina;

				CharacterStats.Speed -= CurrentArmor->Speed;
				AStudyCharacter* CharacterRef = Cast<AStudyCharacter>(this->GetPawn());
				if(CharacterRef)
				{
					CharacterRef->GetCharacterMovement()->MaxWalkSpeed = float(CharacterStats.Speed);
				}

				CharacterStats.Strenght -= CurrentArmor->Strenght;
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "OOps");
			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Actual: " + Actual.ToString() + ", New: " + New.ToString());
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Data Table not found");
	}
}

void AStudyPlayerState::Client_updateCharacterStats_Implementation(FItemDetailsDataTable Actual, FItemDetailsDataTable New)
{
	Server_updateCharacterStats(Actual, New);
}

void AStudyPlayerState::Server_updateCharacterStats_Implementation(FItemDetailsDataTable Actual, FItemDetailsDataTable New)
{
	if (GetLocalRole() == ROLE_Authority  || (GetLocalRole() == ROLE_Authority && GetRemoteRole() < ROLE_AutonomousProxy))
	{
		CharacterStats.ActualLife += New.Life - Actual.Life;
		CharacterStats.FullLife += New.Life - Actual.Life;

		CharacterStats.ActualMana += New.MagicPoints - Actual.MagicPoints;
		CharacterStats.FullMana += New.MagicPoints - Actual.MagicPoints;

		CharacterStats.ActualStamina += New.Stamina - Actual.Stamina;
		CharacterStats.FullStamina += New.Stamina - Actual.Stamina;

		CharacterStats.Speed += New.Speed - Actual.Speed;

		AStudyCharacter* CharacterRef = Cast<AStudyCharacter>(this->GetPawn());
		if (CharacterRef)
		{
			// update the speed based on the item properties
			CharacterRef->GetCharacterMovement()->MaxWalkSpeed = float(CharacterStats.Speed);
		}

		CharacterStats.Strenght += New.Strenght - Actual.Strenght;
	}
	else
	{
		Client_updateCharacterStats(Actual, New);
	}
}

bool AStudyPlayerState::Server_updateCharacterStats_Validate(FItemDetailsDataTable Actual, FItemDetailsDataTable New)
{
	return true;
}

// everytime the is alive change it checks if its still alive
void AStudyPlayerState::OnRep_IsAlive()
{
	AStudyCharacter* CharacterRef = Cast<AStudyCharacter>(this->GetPawn());
	if(!bIsAlive && CharacterRef)
	{
		CharacterRef->DisableInput(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	}
}

void AStudyPlayerState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
 {
     Super::GetLifetimeReplicatedProps(OutLifetimeProps);
 
     DOREPLIFETIME(AStudyPlayerState, CharacterStats);
	 DOREPLIFETIME(AStudyPlayerState, bIsAlive);
 }
