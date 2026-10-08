#pragma once

class Application;
class SceneManager;

class SceneBase {
public:
	enum class SCENE {
		NONE = -1,
		PAUSE,
		TITLE,
		GAME,
		MAX
	};

	static constexpr const char* SCENE_NAME[static_cast<int>(SCENE::MAX)] {
		"Pause",
		"Title",
		"Game",
	};

	SceneBase();
	virtual ~SceneBase() {}

	// シーン名登録
	bool Assign(SCENE scene, int var);
	// 初期化
	virtual bool Init() = 0;
	// 更新
	virtual void Update() = 0;
	// 二次更新
	virtual void LateUpdate() {}
	// 描画
	virtual void Draw() = 0;
	// オーバーレイ描画
	virtual void DrawUI() = 0;
	// 解放
	virtual bool Release() { return false; }

	SCENE GetMyScene() const;
	SCENE GetNextScene() const;

	void ResetNextScene();

protected:
	// シングルトン参照
	Application& app_;
	// シングルトン参照
	SceneManager& scnMng_;

	// 自分のシーン
	SCENE myScene_;
	// 遷移先のシーン
	SCENE nextScene_;
	// 変数(開始ステージ数や初期カーソル位置などを指定する)
	unsigned int variable_;

};