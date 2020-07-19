// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CustomVariables.h"
#include "InventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnClickOnPickupSignature, class AStudyPC*, PlayerController, class APickup*, Pickup, FVector2D, ScreenPosition);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPickupItemSignature, class APickup*, Item, class AStudyPlayerState*, PSRef);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDropItemSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUpdateInventorySignature, TArray<FItemDetailsDataTable>, InventoryItems);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUpdateArmorSetSignature, TArray<FItemDetailsDataTable>, ArmorSetProperties);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class STUDY_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	UPROPERTY(BlueprintReadOnly, Category = "Owner")
    AActor* MyOwner;
	UPROPERTY(BlueprintReadOnly, Category = "Pickup")
	bool bFoundSomething;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widgets")
	TSubclassOf<class UItem3DDescription> Item3DWidgetClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widgets")
	TSubclassOf<class UPickupInfo> PickupInfoWidgetClass;
	UPROPERTY(BlueprintReadOnly, Category = "Widgets")
	class UPickupInfo* PickupInfoWidgetRef;

public:
	// Delegates
    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnClickOnPickupSignature OnPickupClick;
	UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnPickupItemSignature OnPickupItem;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnDropItemSignature OnDropItemOnWorld;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnUpdateInventorySignature OnUpdateInventory;
	UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnUpdateArmorSetSignature OnUpdateArmorSet;
	
	UFUNCTION(BlueprintCallable, Category = "Pickup")
	void PickupClosestItem();

	UFUNCTION()
	void CreateItemInfoWidget(class AStudyPC* PlayerController, class APickup* PickupRef, FVector2D ScreenPosition);

	UFUNCTION(Category = "Widgets")
	void PresentItem3D();

	UFUNCTION(BlueprintCallable, Category = "Replication")
    void PickupItem(class APickup* Item);

	UFUNCTION(Server, Reliable, WithValidation, Category = "Inventory")
    void DropItemOnWorld(TSubclassOf<AActor> PickupClass, FTransform Location, ESlotType SlotType, int32 SlotID);

	UFUNCTION()
	void UpdateInventory(class AStudyPC* PlayerController, class UInventoryDragDropOperation* Operation);

	UFUNCTION()
    void UpdateArmorset(class AStudyCharacter* Character, class UInventoryDragDropOperation* Operation);
	
protected:
	UFUNCTION(Server, Reliable, WithValidation)
    void Server_PickupItem(class APickup* Item, class AStudyPlayerState* PSRef);
};
