// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionGameStatic.h"


const UItemStaticData* UActionGameStatic::GetItemStaticData(TSubclassOf<UItemStaticData> ItemDataClass)
{
	if (IsValid(ItemDataClass))
	{
		//返回同一个默认对象的引用，不生成新对象
		return  GetDefault<UItemStaticData>(ItemDataClass);
	}

	return nullptr;
}
