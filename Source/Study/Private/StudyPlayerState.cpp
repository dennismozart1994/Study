// Fill out your copyright notice in the Description page of Project Settings.

#include "StudyPlayerState.h"
#include "StudyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

AStudyPlayerState::AStudyPlayerState()
{
    // Gameplay Variables
	 CharacterStats.CharacterName =  "TEST CHARACTER";
	 CharacterStats.Strenght = 30;
	 CharacterStats.Speed = 200;
	 CharacterStats.ActualLife = 100;
	 CharacterStats.FullLife = 100;
	 CharacterStats.ActualMana = 1000;
	 CharacterStats.FullMana = 1000;
	 CharacterStats.ActualStamina = 100;
	 CharacterStats.FullStamina = 100;
	 CharacterStats.CurrentLevel = 1;
	 CharacterStats.GoldAmount = 1;
	 CharacterStats.GoNextLevelWhen = 100;
}

void AStudyPlayerState::updateCharacterStats_newTry(UClass* Actual, UClass* New)
{
	// swap between armorset item
	APickup* test1 = Cast<APickup>(Actual);
	APickup* test2 = Cast<APickup>(New);

	if(test1 != NULL && test2 != NULL)
	{
		FItemDetails NewArmorSet = test2->ItemInfo;
		FItemDetails CurrentArmor = test1->ItemInfo;

		CharacterStats.ActualLife += NewArmorSet.Life - CurrentArmor.Life;
		CharacterStats.FullLife += NewArmorSet.Life - CurrentArmor.Life;

		CharacterStats.ActualMana += NewArmorSet.MagicPoints - CurrentArmor.MagicPoints;
		CharacterStats.FullMana += NewArmorSet.MagicPoints - CurrentArmor.MagicPoints;

		CharacterStats.ActualStamina += NewArmorSet.Stamina - CurrentArmor.Stamina;
		CharacterStats.FullStamina += NewArmorSet.Stamina - CurrentArmor.Stamina;

		CharacterStats.Strenght += NewArmorSet.Strenght - CurrentArmor.Strenght;

		CharacterStats.Speed += NewArmorSet.Speed - CurrentArmor.Speed;
		AStudyCharacter* CharacterRef = Cast<AStudyCharacter>(this->GetPawn());
		if(CharacterRef)
		{
			CharacterRef->GetCharacterMovement()->MaxWalkSpeed = float(CharacterStats.Speed);
		}
	}
	// Add new armorset into an empty Slot
	else if(test1 == NULL && test2 != NULL)
	{
		FItemDetails NewArmorSet = test2->ItemInfo;

		CharacterStats.ActualLife += NewArmorSet.Life;
		CharacterStats.FullLife += NewArmorSet.Life;

		CharacterStats.ActualMana += NewArmorSet.MagicPoints;
		CharacterStats.FullMana += NewArmorSet.MagicPoints;

		CharacterStats.ActualStamina += NewArmorSet.Stamina;
		CharacterStats.FullStamina += NewArmorSet.Stamina;

		CharacterStats.Strenght += NewArmorSet.Strenght;

		CharacterStats.Speed += NewArmorSet.Speed;
		AStudyCharacter* CharacterRef = Cast<AStudyCharacter>(this->GetPawn());
		if(CharacterRef)
		{
			CharacterRef->GetCharacterMovement()->MaxWalkSpeed = float(CharacterStats.Speed);
		}
	}
	// remove armorset
	else if(test1 != NULL && test2 != NULL)
	{
		FItemDetails CurrentArmor = test1->ItemInfo;

		CharacterStats.ActualLife -= CurrentArmor.Life;
		CharacterStats.FullLife -= CurrentArmor.Life;

		CharacterStats.ActualMana -= CurrentArmor.MagicPoints;
		CharacterStats.FullMana -= CurrentArmor.MagicPoints;

		CharacterStats.ActualStamina -= CurrentArmor.Stamina;
		CharacterStats.FullStamina -= CurrentArmor.Stamina;

		CharacterStats.Strenght -= CurrentArmor.Strenght;

		CharacterStats.Speed -= CurrentArmor.Speed;
		AStudyCharacter* CharacterRef = Cast<AStudyCharacter>(this->GetPawn());
		if(CharacterRef)
		{
			CharacterRef->GetCharacterMovement()->MaxWalkSpeed = float(CharacterStats.Speed);
		}
	}
	// WTF is happening?
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Both Null");
	}
}

void AStudyPlayerState::updateCharacterStats(FItemDetails CurrentArmor, FItemDetails NewArmorSet)
{
	CharacterStats.ActualLife += NewArmorSet.Life - CurrentArmor.Life;
	CharacterStats.FullLife += NewArmorSet.Life - CurrentArmor.Life;

	CharacterStats.ActualMana += NewArmorSet.MagicPoints - CurrentArmor.MagicPoints;
	CharacterStats.FullMana += NewArmorSet.MagicPoints - CurrentArmor.MagicPoints;

	CharacterStats.ActualStamina += NewArmorSet.Stamina - CurrentArmor.Stamina;
	CharacterStats.FullStamina += NewArmorSet.Stamina - CurrentArmor.Stamina;

	CharacterStats.Speed += NewArmorSet.Speed - CurrentArmor.Speed;
	AStudyCharacter* CharacterRef = Cast<AStudyCharacter>(this->GetPawn());
	if(CharacterRef)
	{
		CharacterRef->GetCharacterMovement()->MaxWalkSpeed = float(CharacterStats.Speed);
	}

	CharacterStats.Strenght += NewArmorSet.Strenght - CurrentArmor.Strenght;
}

void AStudyPlayerState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
 {
     Super::GetLifetimeReplicatedProps(OutLifetimeProps);
 
     DOREPLIFETIME(AStudyPlayerState, CharacterStats);
 }