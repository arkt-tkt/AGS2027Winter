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

	void Update();
	void LateUpdate();
	void Draw();
	void DrawUI();

	void RefillEquipped() { inv_[nowSlot_]->Refill(); }
	float GetReloadProg() { return inv_[nowSlot_]->GetReloadProg(); }

	void Switch(SLOT slot);
	void Add(SLOT slot, bool replace = false);

private:
	Player& player_;
	std::map<SLOT, std::unique_ptr<ItemBase>> inv_;

	SLOT nowSlot_;

};