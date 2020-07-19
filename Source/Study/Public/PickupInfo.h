// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PickupInfo.generated.h"

/**
 * 
 */
UCLASS()
class STUDY_API UPickupInfo : public UUserWidget
{
	GENERATED_BODY()

	public:
	// Default Layout information
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UButton* CollectButton;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UButton* PreviewButton;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UButton* CancelButton;
	
	UPROPERTY(BlueprintReadOnly, Category = Pickups)
	class APickup* PickupReference;

	UFUNCTION()
    void OnCollectClick();

	UFUNCTION()
    void OnPreviewClick();

	UFUNCTION()
    void OnCancelClick();

	protected:
    virtual bool Initialize() override;
	virtual void NativeConstruct() override;
	
};
