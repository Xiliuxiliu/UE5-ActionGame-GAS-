// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionGas/ActionGameTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ActionGameStatic.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONGAS_API UActionGameStatic : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure,BlueprintCallable)
	static const UItemStaticData* GetItemStaticData(TSubclassOf<UItemStaticData> ItemDataClass);
};
