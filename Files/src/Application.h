#pragma once
#include <string>

class Application {
public:
	static void CreateInstance() { if (instance_ == nullptr) instance_ = new Application; }
	static Application& GetInstance() { return *instance_; }
	static void DeleteInstance() { if (instance_ != nullptr) delete instance_; instance_ = nullptr; }

private:
	static Application* instance_;

	Application();
	~Application() {}

	Application(const Application&) = delete;
	Application& operator=(const Application&) = delete;
	Application(Application&&) = delete;
	Application& operator=(Application&&) = delete;

public:
	// 入力解像度
	static const int RESOLUTION_WIDTH = 1920;
	static const int RESOLUTION_HEIGHT = 1080;

	// 機能使用フラグ
	static const bool USE_3D_FLAG;

	// リソースファイルパス
	static const std::string PATH_RESOURCE;
	static const std::string PATH_BGM;
	static const std::string PATH_EFFECT;
	static const std::string PATH_FONT;
	static const std::string PATH_IMAGE;
	static const std::string PATH_MODEL;
	static const std::string PATH_SE;
	static const std::string PATH_SHADER;
	static const std::string PATH_TEXT;

	// 初期化
	bool Init();

	// ゲームループ
	void GameLoop();

	// 解放
	bool Release();

	// ゲームループ離脱
	void Exit();

	// 入力解像度のスクリーンを取得
	int GetPreDrawScreen() const;

private:
	// ゲームループの離脱フラグ
	bool exit_ = false;

	// 入力解像度のスクリーン
	int preDrawScreen_ = -1;

	// システム初期化
	bool InitSystem();

	// Effekseerの初期化
	bool InitEffekseer();

	// 更新処理
	void Update();

	// 描画処理
	void Draw();

};
