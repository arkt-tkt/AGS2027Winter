#include "ResourceManager.h"
#include "ItemManager.h"

void ItemManager::LoadItemsData()
{
	auto& rawData = ResourceManager::GetInstance().Reload(ResourceManager::SRC::JSON_ITEMS).json_;


}
