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
		IMAGE_LOGO,
		IMAGE_TITLE,

		MODEL_STAGE,
		MODEL_SKYDOME,
		MODEL_PLAYER,

		MAX
	};

	// 初期化
	void Init();

	// 解放
	void Release();

	// リソースのロード
	const Resource& Load(SRC src);

	// リソースの再ロード
	const Resource& Reload(SRC src);

	// リソースの複製ロード(モデル用)
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