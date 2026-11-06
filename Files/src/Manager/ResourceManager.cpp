#include <DxLib.h>
#include "ResourceManager.h"

ResourceManager* ResourceManager::instance_ = nullptr;

const std::string ResourceManager::PATH_RESOURCE = "Data/";
const std::string ResourceManager::PATH_SOUND = PATH_RESOURCE + "Sound/";

const std::string ResourceManager::PATH_FONT = ResourceManager::PATH_RESOURCE + "Font/";
const std::string ResourceManager::PATH_IMAGE = ResourceManager::PATH_RESOURCE + "Image/";
const std::string ResourceManager::PATH_MODEL = ResourceManager::PATH_RESOURCE + "Model/";
const std::string ResourceManager::PATH_MUSIC = ResourceManager::PATH_SOUND + "Music/";
const std::string ResourceManager::PATH_SE = ResourceManager::PATH_SOUND + "SE/";
const std::string ResourceManager::PATH_SHADER = ResourceManager::PATH_RESOURCE + "Shader/";
const std::string ResourceManager::PATH_TEXT = ResourceManager::PATH_RESOURCE + "Text/";
const std::string ResourceManager::PATH_VE = ResourceManager::PATH_RESOURCE + "VE/";

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

	// 記述例
	//res = std::make_unique<RES>(RES_T::IMAGE, "Image.png");
	//resourcesMap_.emplace(SRC::IMAGE, std::move(res));
	
	// フォント
	res = std::make_unique<RES>(RES_T::FONT, "851Gkktt_005.ttf", "851ゴチカクット", 24, -1, 1);
	resourcesMap_.emplace(SRC::FONT_UI, std::move(res));

	// モデル
	res = std::make_unique<RES>(RES_T::MODEL, "Stage/fruzer-city/city.mv1");
	resourcesMap_.emplace(SRC::MODEL_STAGE, std::move(res));

	res = std::make_unique<RES>(RES_T::MODEL, "Stage/SkyDome/Skydome.mv1");
	resourcesMap_.emplace(SRC::MODEL_SKYDOME, std::move(res));

	res = std::make_unique<RES>(RES_T::MODEL, "Object/low-poly-ammo-can/source/ammo_can.mv1");
	resourcesMap_.emplace(SRC::MODEL_AMMO_CRATE, std::move(res));

	res = std::make_unique<RES>(RES_T::MODEL, "Object/low-poly-generator-v1.1/Generator.mv1");
	resourcesMap_.emplace(SRC::MODEL_GENERATOR, std::move(res));
	
	res = std::make_unique<RES>(RES_T::MODEL, "Chara/Player/character.mv1");
	resourcesMap_.emplace(SRC::MODEL_PLAYER, std::move(res));

	res = std::make_unique<RES>(RES_T::MODEL, "Chara/Enemy/character.mv1");
	resourcesMap_.emplace(SRC::MODEL_ENEMY1, std::move(res));

	res = std::make_unique<RES>(RES_T::MODEL, "Weapon/sg553-assault-rifle/source/sg553.mv1");
	resourcesMap_.emplace(SRC::MODEL_WPN_AR_STG556, std::move(res));

	// アニメーション
	res = std::make_unique<RES>(RES_T::MODEL, "Chara/Animation/standing_idle.mv1");
	resourcesMap_.emplace(SRC::ANIME_PL_IDLE, std::move(res));

	res = std::make_unique<RES>(RES_T::MODEL, "Chara/Animation/rifle_aiming_idle.mv1");
	resourcesMap_.emplace(SRC::ANIME_PL_RF_IDLE, std::move(res));

	res = std::make_unique<RES>(RES_T::MODEL, "Chara/Animation/walking.mv1");
	resourcesMap_.emplace(SRC::ANIME_PL_RF_MOVE, std::move(res));

	res = std::make_unique<RES>(RES_T::MODEL, "Chara/Animation/reloading.mv1");
	resourcesMap_.emplace(SRC::ANIME_PL_RF_RELOAD, std::move(res));

	// 音楽
	res = std::make_unique<RES>(RES_T::MUSIC, "異次元空間.mp3", 0.8f, true);
	resourcesMap_.emplace(SRC::MUSIC_BGM, std::move(res));

	// 効果音
	res = std::make_unique<RES>(RES_T::MUSIC, "パソコンの電源を入れる.mp3", 1.0f, true);
	resourcesMap_.emplace(SRC::MUSIC_CP_ACTIVATE_0, std::move(res));
	res = std::make_unique<RES>(RES_T::MUSIC, "パソコンの電源を入れる.mp3", 1.0f, true);
	resourcesMap_.emplace(SRC::MUSIC_CP_ACTIVATE_1, std::move(res));
	res = std::make_unique<RES>(RES_T::MUSIC, "パソコンの電源を入れる.mp3", 1.0f, true);
	resourcesMap_.emplace(SRC::MUSIC_CP_ACTIVATE_2, std::move(res));
	res = std::make_unique<RES>(RES_T::MUSIC, "パソコンの電源を入れる.mp3", 1.0f, true);
	resourcesMap_.emplace(SRC::MUSIC_CP_ACTIVATE_3, std::move(res));
	res = std::make_unique<RES>(RES_T::MUSIC, "パソコンの電源を入れる.mp3", 1.0f, true);
	resourcesMap_.emplace(SRC::MUSIC_CP_ACTIVATE_4, std::move(res));

	// 効果音
	res = std::make_unique<RES>(RES_T::SE, "狙撃銃発射.mp3", 1.0f, false);
	resourcesMap_.emplace(SRC::SE_FIRE_AR, std::move(res));

	// JSONファイル
	res = std::make_unique<RES>(RES_T::JSON, "PathFind.json");
	resourcesMap_.emplace(SRC::JSON_PATHFIND, std::move(res));
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
	const auto& lPair = resourcesMap_.find(src);
	if (lPair == resourcesMap_.end())
	{
		return dummy_;
	}

	const auto& rPair = loadedMap_.find(src);
	if (rPair != loadedMap_.end())
	{
		rPair->second.Release();
		loadedMap_.erase(rPair);
	}

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
			pair.second->type_ == Resource::TYPE::FONT ||
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
			pair.second->type_ == Resource::TYPE::FONT ||
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
