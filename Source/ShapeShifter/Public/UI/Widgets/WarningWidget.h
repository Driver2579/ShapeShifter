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
	void SetMessenge(const FString& Messenge);

	UButton* GetOkButton();

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* MessageTextBlock;

	UPROPERTY(meta = (BindWidget))
	UButton* OKButton;

	UPROPERTY(meta = (BindWidget))
	UButton* CancelButton;

private:
	UFUNCTION()
	void OnOK();

	UFUNCTION()
	void OnCancel();
};