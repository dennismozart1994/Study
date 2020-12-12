// Fill out your copyright notice in the Description page of Project Settings.

#include "CustomVariables.h"
const FName ACustomVariables::TABLE_REFERENCE = "/Game/UI/Strings/CommonWords.CommonWords";
const FString ACustomVariables::EQUIP_KEY = "Equip";
// Sets default values
ACustomVariables::ACustomVariables()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

