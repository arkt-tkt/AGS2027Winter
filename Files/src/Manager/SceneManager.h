#pragma once
#include <chrono>
#include <list>
#include <memory>
#include "FPSManager.h"
#include "../Common/Fader.h"
#include "../Object/Camera.h"
#include "../Scene/SceneBase.h"

class SceneManager {
public:
	static void CreateInstance() { if (instance_ == nullptr) instance_ = new SceneManager; }
	static SceneManager& GetInstance() { return *instance_; }
	static void DeleteInstance() { if (instance_ != nullptr) delete instance_; instance_ = nullptr; }

private:
	static SceneManager* instance_;

	SceneManager() {}
	~SceneManager() {}

	SceneManager(const SceneManager&) = delete;
	SceneManager& operator=(const SceneManager&) = delete;
	SceneManager(SceneManager&&) = delete;
	SceneManager& operator=(SceneManager&&) = delete;

public:
	bool Init();
	void Update();
	void Draw();
	bool Release();

	// 一番上(末尾)のシーンを解放
	void ReleaseTopScene();

	// 最前面(アクティブ状態)のシーンを生ポインタ状態で返す
	SceneBase* GetTopScene() const;

	// カメラのポインタを返す
	Camera& GetCameraPtr() const { return *camera_; }

	// フェーダのポインタを返す
	Fader& GetFaderPtr() const { return *fader_; }

	// float型のデルタタイムを返す
	const float GetDeltaTime() const { return FPSManager::GetInstance().GetDeltaTime<float>(); }

	bool IsPause() const;
	bool PrevPause() const;

	void SetNextSceneVariable(unsigned int);
	unsigned int GetNextSceneVariable() const;

private:
	// シーン管理用リスト
	std::list<std::unique_ptr<SceneBase>> sceneList_;
	
	// カメラ
	std::unique_ptr<Camera> camera_;
	// フェーダ
	std::unique_ptr<Fader> fader_;

	// 次シーン用変数
	unsigned int nextSceneVariable_;

	bool isPause_;
	bool prevPause_;
	
	// 切り替え待ち中のシーンのID
	SceneBase::SCENE waitSceneId_;

	// クラス初期化
	bool InitClass();
	// パラメータ初期化
	void InitParam();

	// シーン切り替え処理の準備
	void ChangeScene(SceneBase::SCENE);

	// Faderが動き終わっている場合にtrueを返す
	// (例外を作れば稼働中でもtrueを返せる)
	bool CheckFader();

	// シーン切り替え処理
	void DoChangeScene(SceneBase::SCENE);

};