#pragma once
#include <map>
#include <string>
#include "../Object/Actor/Item/ItemBase.h"

class ItemManager
{
public:
	// シングルトン
	static void CreateInstance()
	{
		if (instance_ == nullptr)
			instance_ = new ItemManager();
	}

	static ItemManager& GetInstance()
	{
		if (instance_ == nullptr)
			instance_ = new ItemManager();
		return *instance_;
	}

	static void DeleteInstance()
	{
		if (instance_ != nullptr)
			delete instance_;
		instance_ = nullptr;
	}

private:
	// インスタンス
	static ItemManager* instance_;

	// シングルトン
	ItemManager(const ItemManager&) = delete;
	ItemManager& operator=(const ItemManager&) = delete;
	ItemManager(ItemManager&&) = delete;
	ItemManager& operator=(ItemManager&&) = delete;

	// コンストラクタ
	ItemManager() {}
	// デストラクタ
	virtual ~ItemManager() {}

public:
	static constexpr const char* CAT_NAME[] =
	{
		"Weap", "Thro" ,"Auxi", "Batt"
	};

	static constexpr const char* BUFF_NAME[] =
	{
		"HP", "Shield"
	};

	void LoadItemData();
	ItemBase::ItemData GetItemData(const std::string& name) const;

private:
	std::map<std::string, ItemBase::ItemData> data_;

	void _LoadValue();
	void _LoadDesc();

};