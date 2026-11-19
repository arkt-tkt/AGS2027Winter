#pragma once
#include <map>
#include <memory>
#include "../Item/ItemBase.h"

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
	};

	void Add(SLOT slot, bool replace);

private:
	std::map<SLOT, std::unique_ptr<ItemBase>> inv_;

};