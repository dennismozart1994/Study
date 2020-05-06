// Fill out your copyright notice in the Description page of Project Settings.

#include "CustomVariables.h"

// Sets default values
ACustomVariables::ACustomVariables()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

FItemDetailsDataTable ACustomVariables::createItemStruct()
{
	FItemDetailsDataTable nullstruct;
	nullstruct.Amount = 0;
	nullstruct.Strenght = 0;
	nullstruct.Speed = 0;
	nullstruct.MagicPoints = 0;
	nullstruct.Stamina = 0;
	nullstruct.GoldLevelRequired = 0;
	nullstruct.Life = 0;
	nullstruct.SellingPrice = 0;
	nullstruct.Mesh = nullptr;
	nullstruct.Thumbnail = nullptr;
	nullstruct.ArmorThumbnail = nullptr;
	nullstruct.ItemType = EItemType::IT_Craft;
	nullstruct.ArmorType = EArmorType::AT_None;
	nullstruct.WeaponType.SocketToAttach = FName("");
	nullstruct.WeaponType.WeaponType = EWeaponType::WT_None;
	nullstruct.DesiredThumbSize = FVector(0.f);
	return nullstruct;
}

FSkilDataTable ACustomVariables::createSkillStruct()
{
	FSkilDataTable nullstruct;
	nullstruct.Name = FText::FromString("Empty");
	nullstruct.MontageToPlay = nullptr;
	nullstruct.Projectile = nullptr;
	nullstruct.SkillThumbnail = nullptr;
	return nullstruct;
}

