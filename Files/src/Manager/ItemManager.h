#pragma once

class ItemManager
{
public:
	// シングルトン
	static void CreateInstance() { if (instance_ == nullptr) instance_ = new ItemManager(); }
	static ItemManager& GetInstance() { return *instance_; }
	static void DeleteInstance() { if (instance_ != nullptr) delete instance_; instance_ = nullptr; }

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
	void LoadItemsData();

};