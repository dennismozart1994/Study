// Fill out your copyright notice in the Description page of Project Settings.

#include "Item3DDescription.h"
#include "UMG/Public/Blueprint/WidgetTree.h"
#include "UMG/Public/Components/Button.h"
#include "Item3DPreview.h"

void UItem3DDescription::OnCloseWidget()
{
	if (Item3DPreviewRef)
	{
		Item3DPreviewRef->Close3D();
		RemoveFromParent();
	}
}

void UItem3DDescription::NativeConstruct()
{
	if (WidgetTree)
	{
		Item3DPreviewRef = Cast<UItem3DPreview>(WidgetTree->FindWidget(FName("3DPreview")));
		UButton* CloseRef = Cast<UButton>(WidgetTree->FindWidget(FName("Close")));
		if (Item3DPreviewRef && CloseRef)
		{
			Item3DPreviewRef->Close3D();
			CloseRef->OnClicked.AddDynamic(this, &UItem3DDescription::OnCloseWidget);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Preview or button not found"));
		}
	}
}
