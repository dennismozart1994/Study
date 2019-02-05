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

	return nullstruct;
}

