#include "ResourceManager.h"
#include "ItemManager.h"

ItemManager* ItemManager::instance_ = nullptr;

void ItemManager::LoadItemData()
{
	_LoadValue();

	_LoadDesc();
}

ItemBase::ItemData ItemManager::GetItemData(const std::string& name) const
{
	auto it = data_.find(name);

	if (it != data_.end())
	{
		return it->second;
	}

	return ItemBase::ItemData();
}

void ItemManager::_LoadValue()
{
	auto& rawData = ResourceManager::GetInstance().Reload(ResourceManager::SRC::JSON_ITEMS).json_;

	for (auto& [key, data] : rawData.items())
	{
		ItemBase::ItemData item;

		if (data.contains("Category"))
		{
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
		}
		else continue;

		if (data.contains("Model"))
			item.model = data["Model"].get<int>();

		if (data.contains("Scale"))
		{
			if (data["Scale"].is_array() && data["Scale"].size() == 3)
			{
				item.scale = Vector3(
					data["Scale"][0].get<float>(),
					data["Scale"][1].get<float>(),
					data["Scale"][2].get<float>());
			}
			else
			{
				item.scale = data["Scale"].get<float>();
			}
		}

		if (data.contains("Offset"))
		{
			if (data["Offset"].is_array() && data["Offset"].size() == 3)
			{
				item.offset = Vector3(
					data["Offset"][0].get<float>(),
					data["Offset"][1].get<float>(),
					data["Offset"][2].get<float>());
			}
			else
			{
				item.offset = data["Offset"].get<float>();
			}
		}

		if (data.contains("Price"))
			item.price = data["Price"].get<int>();

		if (data.contains("Movement"))
			item.movement = data["Movement"].get<float>();

		if (data.contains("Firemode"))
			item.firemode = data["Firemode"].get<std::vector<int>>();

		if (data.contains("Chambering"))
			item.chambering = data["Chambering"].get<int>();

		if (data.contains("Damage"))
			item.damage = data["Damage"].get<float>();

		if (data.contains("Penetration"))
			item.penetration = data["Penetration"].get<int>();

		if (data.contains("Shots"))
			item.shots = data["Shots"].get<int>();

		if (data.contains("Clipsize"))
			item.clipsize = data["Clipsize"].get<int>();

		if (data.contains("Reserve"))
			item.reserve = item.clipsize * data["Reserve"].get<int>();

		if (data.contains("Dispersion"))
			item.dispersion = data["Dispersion"].get<float>();


		if (data.contains("RPM"))
		{
			item.fireRate.clear();

			if (data["RPM"].is_array())
			{
				for (auto& rpm : data["RPM"])
				{
					item.fireRate.emplace_back(60.0f / rpm.get<float>());
				}
			}
			else
			{
				item.fireRate.emplace_back(60.0f / data["RPM"].get<float>());
			}
		}

		if (data.contains("Velocity"))
			item.velocity = data["Velocity"].get<float>();

		if (data.contains("Reload"))
			item.reload = data["Reload"].get<float>();

		if (data.contains("Recoilkick"))
		{
			if (data["Recoilkick"].is_array() && data["Recoilkick"].size() == 3)
			{
				item.recoilKick = Vector3(
					data["Recoilkick"][0].get<float>(),
					data["Recoilkick"][1].get<float>(),
					data["Recoilkick"][2].get<float>());
			}
		}

		if (data.contains("Recoilshake"))
		{
			if (data["Recoilshake"].is_array() && data["Recoilshake"].size() == 3)
			{
				item.recoilShake = Vector3(
					data["Recoilshake"][0].get<float>(),
					data["Recoilshake"][1].get<float>(),
					data["Recoilshake"][2].get<float>());
			}
		}

		data_.emplace(key, item);
	}
}

void ItemManager::_LoadDesc()
{
	auto& rawData = ResourceManager::GetInstance().Reload(ResourceManager::SRC::JSON_ITEMDESCS).json_;

	for (auto& [key, value] : data_)
	{
		if (rawData.contains(key.c_str()))
		{
			auto& data = rawData[key.c_str()];

			if (data.contains("Name"))
				value.name = data["Name"].get<std::string>();

			if (data.contains("Description"))
				value.description = data["Description"].get<std::string>();
		}
	}
}
