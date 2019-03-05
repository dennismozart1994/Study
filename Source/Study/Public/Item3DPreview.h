// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CustomVariables.h"
#include "Item3DPreview.generated.h"

/**
 * 
 */
UCLASS()
class STUDY_API UItem3DPreview : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite, Category = "Item Info")
	FItemDetailsDataTable ItemDetails;

	UPROPERTY(BlueprintReadOnly, Category = "3D Preview")
	class ARecording3DPreviewActor* ActorToSpawn;

	UPROPERTY(EditAnywhere, Category = "3D Preview")
	TSubclassOf<class ARecording3DPreviewActor> RecordingRef;

	UFUNCTION()
	FVector2D GetMousePosition();

	UFUNCTION()
	void SpawnPreview();

	UFUNCTION()
	void Close3D();

protected:
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual FReply NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
};
