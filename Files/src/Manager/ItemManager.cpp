#include "ResourceManager.h"
#include "ItemManager.h"

void ItemManager::LoadItemsData()
{
	auto& rawData = ResourceManager::GetInstance().Reload(ResourceManager::SRC::JSON_ITEMS).json_;

	for (auto& data : rawData)
	{
		ItemBase::ItemData item;

		auto cat = data["Category"].get<std::string>();

		int ci = 0;
		for (; ci < _countof(CAT_NAME); ++ci)
		{
			if (cat == std::string(CAT_NAME[ci]))
			{
				break;
			}
		}

		//if (ci == _countof(CAT_NAME)) continue;
		if (ci != 0) continue;

		item.name = data["Name"].get<std::string>();
		item.price = data["Price"].get<int>();
		item.movement = data["Movement"].get<float>();
		
		if (data.contains("Firemode"))
			item.firemode = data["Firemode"].get<std::vector<int>>();

		if (data.contains("Damage"))
			item.damage = data["Damage"].get<float>();

		if (data.contains("Shots"))
			item.shots = data["Shots"].get<int>();

		if (data.contains("Clipsize"))
			item.clipsize = data["Clipsize"].get<int>();

		if (data.contains("Reserve"))
			item.reserve = item.clipsize * data["Reserve"].get<int>();

		if (data.contains("Dispersion"))
			item.dispersion = data["Dispersion"].get<float>();

		if (data.contains("RPM"))
			item.fireRate = 60.0f / data["RPM"].get<float>();

		if (data.contains("Velocity"))
			item.velocity = data["Velocity"].get<float>();

		if (data.contains("Reload"))
			item.reload = data["Reload"].get<float>();

		dat_.emplace_back(data);
	}
}
