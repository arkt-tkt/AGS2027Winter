#include <DxLib.h>
#include "../Application.h"
#include "ResourceManager.h"

ResourceManager* ResourceManager::instance_ = nullptr;

ResourceManager::ResourceManager()
{
}

void ResourceManager::DeleteInstance()
{
	if (instance_ == nullptr) return;

	instance_->Release();
	for (auto& res : instance_->resourcesMap_)
	{
		res.second->Release();
	}
	instance_->resourcesMap_.clear();

	delete instance_;
	instance_ = nullptr;
}

void ResourceManager::Init()
{
	// 推奨しませんが、どうしても使いたい方は
	using RES = Resource;
	using RES_T = RES::TYPE;

	std::unique_ptr<Resource> res;

	// 例
	//res = std::make_unique<RES>(RES_T::IMAGE, "Image.png");
	//resourcesMap_.emplace(SRC::IMAGE, std::move(res));
	
	res = std::make_unique<RES>(RES_T::MODEL, "Stage/fruzer-city/city.mv1");
	resourcesMap_.emplace(SRC::MODEL_STAGE, std::move(res));
	
	res = std::make_unique<RES>(RES_T::MODEL, "Stage/SkyDome/Skydome.mv1");
	resourcesMap_.emplace(SRC::MODEL_SKYDOME, std::move(res));
	
	res = std::make_unique<RES>(RES_T::MODEL, "Chara/Player/character.mv1");
	resourcesMap_.emplace(SRC::MODEL_PLAYER, std::move(res));

	res = std::make_unique<RES>(RES_T::MODEL, "Chara/Animation/standing_idle.mv1");
	resourcesMap_.emplace(SRC::ANIME_IDLE, std::move(res));

	res = std::make_unique<RES>(RES_T::MODEL, "Chara/Animation/rifle_aiming_idle.mv1");
	resourcesMap_.emplace(SRC::ANIME_RIFLE_IDLE, std::move(res));
}

void ResourceManager::Release()
{
	for (auto& p : loadedMap_)
	{
		p.second.Release();
	}

	loadedMap_.clear();

	for (auto& str : externalFontList_)
	{
		RemoveFontResourceExA(str.c_str(), FR_PRIVATE, NULL);
	}

	externalFontList_.clear();
}

const Resource& ResourceManager::Load(SRC src)
{
	Resource& res = _Load(src);
	if (res.type_ == Resource::TYPE::NONE)
	{
		return dummy_;
	}
	return res;
}

const Resource& ResourceManager::Reload(SRC src)
{
	const auto& lPair = loadedMap_.find(src);
	if (lPair == loadedMap_.end())
	{
		return dummy_;
	}

	lPair->second.Release();

	const Resource& res = Load(src);
	return res;
}

int ResourceManager::LoadModelDuplicate(SRC src)
{
	Resource& res = _Load(src);
	if (res.type_ != Resource::TYPE::MODEL)
	{
		return -1;
	}

	int duId = MV1DuplicateModel(res.handleId_);
	res.duplicateModelIds_.push_back(duId);

	return duId;
}

void ResourceManager::LoadAllResourcesAsync()
{
	// 登録されている全リソースに対してロード処理を実行
	// (_Load 内部で実際にロード処理が走り、loadedMap_に格納されます)
	for (auto& pair : resourcesMap_)
	{
		if (pair.second->type_ == Resource::TYPE::DXFONT ||
			pair.second->type_ == Resource::TYPE::EFFEKSEER ||
			pair.second->type_ == Resource::TYPE::JSON) continue;

		_Load(pair.first);
	}
}

void ResourceManager::LoadAllResourcesSyncOnly()
{
	for (auto& pair : resourcesMap_)
	{
		if (pair.second->type_ == Resource::TYPE::DXFONT ||
			pair.second->type_ == Resource::TYPE::EFFEKSEER ||
			pair.second->type_ == Resource::TYPE::JSON) _Load(pair.first);
	}
}

const std::map<ResourceManager::SRC, Resource&>& ResourceManager::GetLoadedMap()
{
	return loadedMap_;
}

Resource& ResourceManager::_Load(SRC src)
{
	// ロード済みチェック
	const auto& lPair = loadedMap_.find(src);
	if (lPair != loadedMap_.end())
	{
		return *resourcesMap_.find(src)->second;
	}

	// リソース登録チェック
	const auto& rPair = resourcesMap_.find(src);
	if (rPair == resourcesMap_.end())
	{
		// 登録されていない
		return dummy_;
	}

	Resource& res = *rPair->second;

	// フォントデータ専用処理
	if (res.type_ == Resource::TYPE::FONT)
	{
		// フォントを一時的にOSへ読み込む
		_LoadFontRes(res);
	}

	// ロード処理
	res.Load();

	// 念のためコピーコンストラクタ
	loadedMap_.emplace(src, res);

	return res;
}

void ResourceManager::_LoadFontRes(Resource& res)
{
	bool osLoad = false;

	for (auto& str : externalFontList_)
	{
		if (str == res.path_)
		{
			osLoad = true;
			break;
		}
	}

	if (!osLoad)
	{
		std::string path = res.path_;

		// 一時的にフォントデータを追加する
		AddFontResourceExA(path.c_str(), FR_PRIVATE, NULL);

		externalFontList_.emplace_back(path);
	}
}
