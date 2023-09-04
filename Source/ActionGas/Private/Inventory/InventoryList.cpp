// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/InventoryList.h"

#include "ActionGas/ActionGameTypes.h"
#include "Inventory/InventoryItemInstance.h"

void FInventoryList::AddItem(TSubclassOf<UItemStaticData> InItemStaticDataClass)
{
	//AddDefaulted_GetRef在数组末尾添加一个新项，并返回引用
	FInventoryListItem& Item = Items.AddDefaulted_GetRef();

	//创建背包元素，并赋予指针
	Item.ItemInstance = NewObject<UInventoryItemInstance>();

	//初始化背包元素
	Item.ItemInstance->Init(InItemStaticDataClass);

	//数组元素标记为脏数据，如果数组中添加或更改，必须调用此函数
	MarkItemDirty(Item);
	
}

void FInventoryList::RemoveItem(TSubclassOf<UItemStaticData> InItemStaticDataClass)
{
	for (auto ItemIter = Items.CreateIterator();ItemIter;++ItemIter)
	{
		FInventoryListItem& Item = *ItemIter;
		if (Item.ItemInstance && Item.ItemInstance->GetItemStaticData()->IsA(InItemStaticDataClass))
		{
			//删除
			ItemIter.RemoveCurrent();

			//标记为脏数据
			MarkArrayDirty();
			break;
		}
	}
}

TArray<FInventoryListItem>& FInventoryList::GetItemRef()
{
	return Items;
}
