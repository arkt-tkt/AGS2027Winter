#pragma once
#include <map>
#include <memory>
#include "../Item/ItemBase.h"

class Player;

class Inventory
{
public:
	enum class SLOT
	{
		SLOT_PRIMARY1,
		SLOT_PRIMARY2,
		SLOT_SECONDARY,
		SLOT_BATTERY,
		SLOT_AUXILIARY1,
		SLOT_AUXILIARY2,
		SLOT_MAX
	};

	Inventory(Player* player);
	~Inventory();

	void Init();
	void Update();
	void LateUpdate();
	void Draw();
	void DrawUI();
	void Release();

	void RefillEquipped() { inv_[nowSlot_]->Refill(); }
	float GetReloadProg() { return inv_[nowSlot_]->GetReloadProg(); }
	bool IsActed() const { return isActed_; }

	void SetSharedAwayColliders(std::vector<std::weak_ptr<Collider3D>> cols) { sharedAwayCol_ = cols; }
	const std::pair<const int&, const int&> GetAmmo() { return inv_[nowSlot_]->GetAmmo(); }
	const std::string& GetFiremodeLabel() { return inv_[nowSlot_]->GetFiremodeLabel(); }
	const ItemBase::ItemData& GetData() { return inv_[nowSlot_]->GetData(); }

	void Switch(SLOT slot);
	void Add(SLOT slot, const std::string& name, bool replace = false);

private:
	Player& player_;
	std::map<SLOT, std::unique_ptr<ItemBase>> inv_;

	std::vector<std::weak_ptr<Collider3D>> sharedAwayCol_;

	SLOT nowSlot_;
	bool isActed_ = false;

};