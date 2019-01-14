// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/Classes/Engine/SkeletalMesh.h"
#include "CustomVariables.generated.h"

UENUM(BlueprintType)
enum class ESlotType : uint8
{
	ST_Inventory 	UMETA(DisplayName="Inventory"),
	ST_Skill 		UMETA(DisplayName="Skill"),
	ST_ArmorSet 	UMETA(DisplayName="Armor Set")
};

UENUM(BlueprintType)
enum class EItemType : uint8
{
	IT_Skill 		UMETA(DisplayName="Skill"),
	IT_Recover 		UMETA(DisplayName="Recover"),
	IT_ArmorSet 	UMETA(DisplayName="Armor Set"),
	IT_Craft	 	UMETA(DisplayName="Craft")
};

UENUM(BlueprintType)
enum class EArmorType : uint8
{
	AT_None			UMETA(DisplayName="None"),
	AT_Ring			UMETA(DisplayName="Ring"),
	AT_Helmet		UMETA(DisplayName="Helmet"),
	AT_Grimoire		UMETA(DisplayName="Grimoire"),
	AT_Weapon1		UMETA(DisplayName="Weapon1"),
	AT_Chest		UMETA(DisplayName="Chest"),
	AT_Weapon2		UMETA(DisplayName="Weapon2"),
	AT_Gloves		UMETA(DisplayName="Gloves"),
	AT_Trowsers		UMETA(DisplayName="Trowsers"),
	AT_Shoes		UMETA(DisplayName="Shoes")
};

USTRUCT(BlueprintType)
struct FItemDetails
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item")
	USkeletalMesh* Mesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Gameplay Config")
	EArmorType ArmorType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Gameplay Config")
	EItemType ItemType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Thumbnails")
	UTexture2D* Thumbnail;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Thumbnails")
	UTexture2D* ArmorThumbnail;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item Properties")
	int32 Speed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item Properties")
	int32 MagicPoints;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item Properties")
	int32 Stamina;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item Properties")
	int32 Strenght;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item Properties")
	int32 GoldLevel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item Properties")
	int32 Life;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item Properties")
	int32 amount;
};

USTRUCT(BlueprintType)
struct FMyStats
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Stats)
	FString CharacterName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Stats)
	int32 Strenght;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Stats)
	int32 Speed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Stats)
	int32 ActualLife;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Stats)
	int32 FullLife;
	// add NotReplicated, on uproperty when you desired to some member not replicates
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Stats)
	int32 ActualMana;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Stats)
	int32 FullMana;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Stats)
	int32 ActualStamina;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Stats)
	int32 FullStamina;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Stats)
	int32 CurrentLevel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Stats)
	int32 GoldAmount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Stats)
	int32 GoNextLevelWhen;
};

UCLASS()
class STUDY_API ACustomVariables : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACustomVariables();
};
