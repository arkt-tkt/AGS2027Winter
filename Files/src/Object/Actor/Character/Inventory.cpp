#include "Inventory.h"

void Inventory::Add(SLOT slot, bool replace)
{
	if (!replace)
	{
		auto it = inv_.find(slot);
		if (it == inv_.end()) return;
	}

	std::unique_ptr<ItemBase> newItem = std::make_unique<ItemBase>();
	newItem->Init();

	inv_.emplace(slot, std::move(newItem));
}
