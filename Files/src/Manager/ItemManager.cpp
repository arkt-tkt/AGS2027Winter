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

		if (data.contains("Type"))
		{
			auto cat = data["Type"].get<std::string>();

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

		std::string keyName = "Model";
		if (data.contains(keyName))
			item.model = data[keyName].get<int>();

		keyName = "Scale";
		if (data.contains(keyName))
		{
			if (data[keyName].is_array() && data[keyName].size() == 3)
			{
				item.scale = Vector3(
					data[keyName][0].get<float>(),
					data[keyName][1].get<float>(),
					data[keyName][2].get<float>());
			}
			else
			{
				item.scale = data[keyName].get<float>();
			}
		}

		keyName = "Offset";
		if (data.contains(keyName))
		{
			if (data[keyName].is_array() && data[keyName].size() == 3)
			{
				item.offset = Vector3(
					data[keyName][0].get<float>(),
					data[keyName][1].get<float>(),
					data[keyName][2].get<float>());
			}
			else
			{
				item.offset = data[keyName].get<float>();
			}
		}

		keyName = "Muzzle";
		if (data.contains(keyName))
		{
			if (data[keyName].is_array() && data[keyName].size() == 3)
			{
				item.muzzle = Vector3(
					data[keyName][0].get<float>(),
					data[keyName][1].get<float>(),
					data[keyName][2].get<float>());
			}
			else
			{
				item.muzzle = data[keyName].get<float>();
			}
		}

		keyName = "Price";
		if (data.contains(keyName))
			item.price = data[keyName].get<int>();

		keyName = "Movement";
		if (data.contains(keyName))
			item.movement = data[keyName].get<float>();

		keyName = "FireMode";
		if (data.contains(keyName))
			item.fireMode = data[keyName].get<std::vector<int>>();

		keyName = "Chambering";
		if (data.contains(keyName))
			item.chambering = data[keyName].get<int>();

		keyName = "Damage";
		if (data.contains(keyName))
			item.damage = data[keyName].get<float>();

		keyName = "Penetration";
		if (data.contains(keyName))
			item.penetration = data[keyName].get<int>();

		keyName = "Shots";
		if (data.contains(keyName))
			item.shots = data[keyName].get<int>();

		keyName = "ClipSize";
		if (data.contains(keyName))
			item.clipSize = data[keyName].get<int>();

		keyName = "ReserveClip";
		if (data.contains(keyName))
			item.reserve = item.clipSize * data[keyName].get<int>();

		keyName = "DispersionRad";
		if (data.contains(keyName))
			item.dispersionRad = data[keyName].get<float>();

		keyName = "RPM";
		if (data.contains(keyName))
		{
			item.fireRate.clear();

			if (data[keyName].is_array() && data[keyName].size() <= 2)
			{
				for (auto& rpm : data[keyName])
				{
					item.fireRate.emplace_back(60.0f / rpm.get<float>());
				}
			}
			else
			{
				item.fireRate.emplace_back(60.0f / data[keyName].get<float>());
			}
		}

		keyName = "Velocity";
		if (data.contains(keyName))
			item.velocity = data[keyName].get<float>();

		keyName = "ReloadTime";
		if (data.contains(keyName))
			item.reloadTime = data[keyName].get<float>();

		keyName = "RecoilKick";
		if (data.contains(keyName))
		{
			if (data[keyName].is_array() && data[keyName].size() == 3)
			{
				item.recoilKick = Vector3(
					data[keyName][0].get<float>(),
					data[keyName][1].get<float>(),
					data[keyName][2].get<float>());
			}
		}

		keyName = "RecoilShake";
		if (data.contains(keyName))
		{
			if (data[keyName].is_array() && data[keyName].size() == 3)
			{
				item.recoilShake = Vector3(
					data[keyName][0].get<float>(),
					data[keyName][1].get<float>(),
					data[keyName][2].get<float>());
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
