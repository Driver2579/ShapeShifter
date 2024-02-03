// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WarningWidget.generated.h"

class UButton;

UCLASS()
class SHAPESHIFTER_API UWarningWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// Display this widget on top of the current one
	static const UWarningWidget* Show(UUserWidget* Parent, TSubclassOf<UWarningWidget> WarningWidgetClass);

	// Set message to display
	void SetMessage(const FString& Message) const;

	UButton* GetOkButton() const;

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* MessageTextBlock;

	UPROPERTY(meta = (BindWidget))
	UButton* OkButton;

	UPROPERTY(meta = (BindWidget))
	UButton* CancelButton;

private:
	// Close this menu
	UFUNCTION()
	void CloseWidget();
};