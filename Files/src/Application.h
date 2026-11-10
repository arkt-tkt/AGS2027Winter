#pragma once
#include <memory>
#include <string>
#include "Common/Geometry.h"

class RNG;

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
	// 出力解像度
	static constexpr int RESOLUTION_WIDTH = 1920;
	static constexpr int RESOLUTION_HEIGHT = 1080;
	static constexpr Vector2 RESOLUTION = { RESOLUTION_WIDTH, RESOLUTION_HEIGHT };

	// 機能使用フラグ
	static const bool USE_3D_FLAG;

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

	// 乱数生成器の参照ポインタを取得
	RNG& GetRNGPointer() const;

private:
	std::shared_ptr<RNG> rng_;

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
