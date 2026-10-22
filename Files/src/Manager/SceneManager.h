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
	void ReleaseScene();

	// 最前面(アクティブ状態)のシーンを生ポインタ状態で返す
	SceneBase* GetTopScene() const;

	Camera& GetCameraPtr() const { return *camera_; }

	Fader& GetFaderPtr() const { return *fader_; }

	// float型のデルタタイムを返す
	const float GetDeltaTime() const { return FPSManager::GetInstance().GetDeltaTime<float>(); }

	bool IsPause() const;
	bool PrevPause() const;

	void SetNextSceneVariable(unsigned int);
	unsigned int GetNextSceneVariable() const;

private:
	std::list<std::unique_ptr<SceneBase>> sceneList_;
	std::unique_ptr<Camera> camera_;
	std::unique_ptr<Fader> fader_;

	int postEffectScreen_;

	unsigned int nextSceneVariable_;
	bool isPause_;
	bool prevPause_;
	SceneBase::SCENE waitSceneId_;

	bool InitClass();
	void InitParam();

	// シーン切り替え処理の準備
	void ChangeScene(SceneBase::SCENE);

	// Faderの状態に合わせて他の処理に繋げる
	bool CheckFader();

	// シーン切り替え処理
	void DoChangeScene(SceneBase::SCENE);

};