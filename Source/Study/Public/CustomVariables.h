// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/Classes/Engine/SkeletalMesh.h"
#include "Engine/DataTable.h"
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
	AT_ShoulderPad	UMETA(DisplayName="ShoulderPad"),
	AT_Helmet		UMETA(DisplayName="Helmet"),
	AT_Grimoire		UMETA(DisplayName="Grimoire"),
	AT_Weapon1		UMETA(DisplayName="Weapon1"),
	AT_Chest		UMETA(DisplayName="Chest"),
	AT_Weapon2		UMETA(DisplayName="Weapon2"),
	AT_Gloves		UMETA(DisplayName="Gloves"),
	AT_Belt			UMETA(DisplayName="Belt"),
	AT_Shoes		UMETA(DisplayName="Shoes")
};

UENUM(BlueprintType)
enum class ESkillClass : uint8
{
	SC_None			UMETA(DisplayName = "None"),
	SC_Warrior		UMETA(DisplayName = "Warrior"),
	SC_Archier	 	UMETA(DisplayName = "Archier"),
	SC_Magician		UMETA(DisplayName = "Magician")
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	WT_None			UMETA(DisplayName = "None"),
	WT_Sword	 	UMETA(DisplayName = "Sword"),
	WT_DualBlade	UMETA(DisplayName = "Dual Blade"),
	WT_Axe_OR_Blunt	UMETA(DisplayName = "Axe or Blunt"),
	WT_Shield 		UMETA(DisplayName = "Shield"),
	WT_Bow		 	UMETA(DisplayName = "Bow"),
	WT_Quiver		UMETA(DisplayName = "Quiver"),
	WT_Staff		UMETA(DisplayName = "Staff")
};

UENUM(BlueprintType)
enum class ESkillType : uint8
{
	ST_Spawn		UMETA(DisplayName = "Spawn Item"),
	ST_Buff	 		UMETA(DisplayName = "Buff"),
	ST_Recover		UMETA(DisplayName = "Recover"),
	ST_State		UMETA(DisplayName = "State"),
	ST_Passive		UMETA(DisplayName = "Passive"),
	ST_Action		UMETA(DisplayName = "Action")
};

UENUM(BlueprintType)
enum class ESkillTarget : uint8
{
	ST_Self			UMETA(DisplayName = "Self"),
	ST_Enemy		UMETA(DisplayName = "Enemy"),
	ST_AreaAround	UMETA(DisplayName = "Area Around the Player")
};

UENUM(BlueprintType)
enum class EStateType : uint8
{
	ST_Stun			UMETA(DisplayName = "Paralise enemie"),
	ST_Poison	 	UMETA(DisplayName = "Poison Enemie"),
	ST_Delay		UMETA(DisplayName = "Reduces Enemie Speed"),
	ST_Size			UMETA(DisplayName = "Reduces Enemie Size")
};

UENUM(BlueprintType)
enum class EBuffType : uint8
{
	BT_Speed			UMETA(DisplayName = "Increase Speed for a period of time"),
	BT_Strength			UMETA(DisplayName = "Increase Stenght for a period of time"),
	BT_Life				UMETA(DisplayName = "Increase Life for a period of time"),
	BT_Magic			UMETA(DisplayName = "Increase Mana for a period of time"),
	BT_Stamina			UMETA(DisplayName = "Increase Stamina for a period of time"),
	BT_Defense			UMETA(DisplayName = "Decrease enemy damage by a percentage during an specific amount of time"),
};

// data table structs
USTRUCT(BlueprintType)
struct FSkilDataTable : public FTableRowBase
{
	GENERATED_BODY()

	// UStruct Constructor
	FSkilDataTable(
		ESkillClass tree = ESkillClass::SC_None,
		FName requiresToUnlock = FName("None"),
		float price = 0.f,
		int32 lvl = 0,
		TSubclassOf<class AMasterSkill> skill = nullptr
	) : TreeClass(tree), RequiredSkillToUnlock(requiresToUnlock), PriceToUnlock(price),
	GoldLevelRequired(lvl), SkillClass(skill) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	ESkillClass TreeClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	FName RequiredSkillToUnlock;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	float PriceToUnlock;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	int32 GoldLevelRequired;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Class")
	TSubclassOf<class AMasterSkill> SkillClass;
};

USTRUCT(BlueprintType)
struct FSkillDetails
{
	GENERATED_BODY()
	
	// UStruct Constructor
	FSkillDetails (
        FText name = FText::FromString("None"),
        FText description = FText::FromString("None"),
        UTexture2D* skill_thumbnail = nullptr,
        UParticleSystem* particle = nullptr,
        ESkillTarget target = ESkillTarget::ST_Self,
        ESkillType type = ESkillType::ST_Action,
        int32 mp_cost = 0,
        float cooldown = 5.f,
        UAnimMontage* montage = nullptr,
        int32 life = 0,
        int32 magic_points = 0,
        int32 speed = 0,
        int32 stamina = 0,
        int32 strength = 0,
        EBuffType buffType = EBuffType::BT_Strength,
        float buffingTime = 0.f,
        int32 bf_value = 0,
        EStateType state = EStateType::ST_Stun,
        float stateTime = 0.f) :
    Name(name), Description(description), SkillThumbnail(skill_thumbnail), Particle(particle), Target(target),
    SkillType(type), MPCost(mp_cost), CoolDown(cooldown), MontageToPlay(montage), Life(life), MagicPoints(magic_points),
	Speed(speed), Stamina(stamina), Strength(strength), BuffType(buffType), BuffingTime(buffingTime),
	BuffingValue(bf_value), State(state), StateTime(stateTime) {}

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Description")
    FText Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Description")
    FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Design")
    UTexture2D* SkillThumbnail;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Design")
    UParticleSystem* Particle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Basic Info")
    ESkillTarget Target;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Basic Info")
    ESkillType SkillType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Basic Info")
    int32 MPCost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Basic Info")
    float CoolDown;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Basic Info")
    UAnimMontage* MontageToPlay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "If Recover Skill Type")
    int32 Life;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "If Recover Skill Type")
    int32 MagicPoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "If Recover Skill Type")
    int32 Speed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "If Recover Skill Type")
    int32 Stamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "If Recover Skill Type")
    int32 Strength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "If Buff Skill Type")
    EBuffType BuffType;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "If Buff Skill Type")
    float BuffingTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "If Buff Skill Type")
    int32 BuffingValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "If State Skill Type")
    EStateType State;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "If State Skill Type")
    float StateTime;
};

// simple struct
USTRUCT(BlueprintType)
struct FWeaponType
{
	GENERATED_BODY()

	FWeaponType(
		EWeaponType WType = EWeaponType::WT_None,
		FName Socket = FName("None"),
		TSoftClassPtr<AActor> projectile = nullptr
	) : WeaponType(WType), SocketToAttach(Socket), Projectile(projectile) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Type")
	EWeaponType WeaponType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Socket")
	FName SocketToAttach;

	// Weapon Projectile if there's is one.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	TSoftClassPtr<AActor> Projectile;
};

// data table structs
USTRUCT(BlueprintType)
struct FItemDetailsDataTable : public FTableRowBase
{
	GENERATED_BODY()

	FItemDetailsDataTable(
		FText n = FText::FromString("None"),
		FText d = FText::FromString("None"),
		FVector size = FVector(1.f),
		USkeletalMesh* mesh = nullptr,
		EItemType type = EItemType::IT_Craft,
		EArmorType AType = EArmorType::AT_None,
		FWeaponType WType = FWeaponType(),
		UTexture2D* Image = nullptr,
		UTexture2D* ArmorImage = nullptr,
		int32 s = 0,
		int32 mp = 0,
		int32 sm = 0,
		int32 st = 0,
		int32 lvl = 0,
		int32 lf = 0,
		int32 am = 0,
		int32 slp = 0
	) : Name(n), Description(d), DesiredThumbSize(size), Mesh(mesh), ItemType(type), ArmorType(AType),
	WeaponType(WType), Thumbnail(Image), ArmorThumbnail(ArmorImage), Speed(s), MagicPoints(mp), Stamina(sm),
	Strenght(st), GoldLevelRequired(lvl), Life(lf), Amount(am), SellingPrice(slp) {}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Description")
	FText Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Description")
	FText Description;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Description")
	FVector DesiredThumbSize;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Skeletal Mesh")
	USkeletalMesh* Mesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Gameplay Config")
	EItemType ItemType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Gameplay Config")
	EArmorType ArmorType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay Config")
	FWeaponType WeaponType;
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="$")
	int32 GoldLevelRequired;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item Properties")
	int32 Life;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="$")
	int32 Amount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="$")
	int32 SellingPrice;
};

// simple struct
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
	static const FName TABLE_REFERENCE;
	static const FString EQUIP_KEY;
	// Sets default values for this actor's properties
	ACustomVariables();
};
