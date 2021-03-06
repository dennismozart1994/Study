// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayHUD.generated.h"

/**
 * 
 */
UCLASS()
class STUDY_API UGameplayHUD : public UUserWidget
{
	GENERATED_BODY()
	public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UActionsHUD* Actions;
	
	protected:
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateSlots();

	// Deal native OnDrop Process
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
};
