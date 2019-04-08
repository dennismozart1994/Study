// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CustomVariables.h"
#include "Item3DDescription.generated.h"

/**
 * 
 */
UCLASS()
class STUDY_API UItem3DDescription : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Info")
	class UUserWidget* CurrentSlotRef;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item Info")
	FItemDetailsDataTable ItemDetails;
	UFUNCTION(BlueprintImplementableEvent)
	void OnClickedClose();
	UFUNCTION()
	void OnCloseWidget();

protected:
	virtual void NativeConstruct() override;

	class UItem3DPreview* Item3DPreviewRef;
};
