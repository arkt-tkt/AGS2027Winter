#pragma once
#include <list>
#include <map>
#include <memory>
#include <string>
#include "Resource.h"

class ResourceManager
{

public:
	// 明示的にインステンスを生成する
	static void CreateInstance() { if (instance_ == nullptr) { instance_ = new ResourceManager(); } }
	// 静的インスタンスの取得
	static ResourceManager& GetInstance() { return *instance_; }
	// 静的インスタンスの破棄
	static void DeleteInstance();

private:
	// 静的インスタンス
	static ResourceManager* instance_;

	// デフォルトコンストラクタをprivateにして、
	// 外部から生成できない様にする
	ResourceManager();
	~ResourceManager() = default;

	ResourceManager(const ResourceManager&) = delete;
	ResourceManager& operator=(const ResourceManager&) = delete;
	ResourceManager(ResourceManager&&) = delete;
	ResourceManager& operator=(ResourceManager&&) = delete;

public:
	// リソース名
	enum class SRC
	{
		NONE = -1,

		FONT_UI,

		IMAGE_LOGO = 10,
		IMAGE_TITLE,

		MODEL_STAGE = 20,
		MODEL_SKYDOME,
		MODEL_AMMO_CRATE,
		MODEL_GENERATOR,
		MODEL_PLAYER,
		MODEL_ENEMY_RUSH,
		MODEL_ENEMY_ARMOR,

		ANIME_PL_RF_IDLE = 40,
		ANIME_PL_RF_WALK_F,
		ANIME_PL_RF_WALK_FL,
		ANIME_PL_RF_WALK_FR,
		ANIME_PL_RF_WALK_L,
		ANIME_PL_RF_WALK_R,
		ANIME_PL_RF_WALK_B,
		ANIME_PL_RF_WALK_BL,
		ANIME_PL_RF_WALK_BR,
		ANIME_PL_RF_DASH_FRONT,
		ANIME_PL_RF_JUMP,
		ANIME_PL_RF_RELOAD,
		ANIME_EN_IDLE,
		ANIME_EN_MOVE,
		ANIME_EN_JUMP,
		ANIME_EN_PUNCH,
		
		MUSIC_BGM = 70,
		MUSIC_CP_ACTIVATE_0,
		MUSIC_CP_ACTIVATE_1,
		MUSIC_CP_ACTIVATE_2,
		MUSIC_CP_ACTIVATE_3,
		MUSIC_CP_ACTIVATE_4,

		SE_SWITCH_FIREMODE = 80,
		SE_FIRE_AR,

		SHADER_GAUSS = 90,
		SHADER_SCREEN,

		EFFEKSEER_BULLET = 100,

		JSON_ENEMIES = 110,
		JSON_ITEMS,
		JSON_ITEMDESCS,
		JSON_PATHFIND,

		MODEL_WPN_AR_SG553 = 120,
		MODEL_WPN_SMG_MP5K,
		MODEL_WPN_SG_GL06,
		MODEL_WPN_SR_SCOUT,
		MODEL_WPN_MG_MG3,

		MAX
	};

	// リソースファイルパス
	static const std::string PATH_RESOURCE;
	static const std::string PATH_SOUND;

	static const std::string PATH_FONT;
	static const std::string PATH_IMAGE;
	static const std::string PATH_MODEL;
	static const std::string PATH_MUSIC;
	static const std::string PATH_SE;
	static const std::string PATH_SHADER;
	static const std::string PATH_TEXT;
	static const std::string PATH_VE;

	// 初期化
	void Init();

	// 解放
	void UnLoad();

	// リソースのロード
	const Resource& Load(SRC src);

	// リソースの再ロード
	const Resource& Reload(SRC src);

	// リソースの複製ロード(モデル用)
	// @return 複製したモデルのハンドルID
	int LoadModelDuplicate(SRC src);

	// 全リソースの一括非同期ロード
	void LoadAllResourcesAsync();
	// 同期ロード専用リソースの一括ロード
	void LoadAllResourcesSyncOnly();

	// 読み込み済みのリソース表を返す
	const std::map<SRC, Resource&>& GetLoadedMap();

private:
	// リソース管理の対象
	std::map<SRC, std::unique_ptr<Resource>> resourcesMap_;

	// 読み込み済みリソース
	std::map<SRC, Resource&> loadedMap_;

	// フォント
	std::list<std::string> externalFontList_;

	// 読み込み失敗時用のダミー
	Resource dummy_;

	// 内部ロード
	Resource& _Load(SRC src);
	// OSに対するフォントの一時的読み込み
	void _LoadFontRes(Resource& res);

};