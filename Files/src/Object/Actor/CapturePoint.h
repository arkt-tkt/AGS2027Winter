#pragma once
#include "Character/CharacterBase3D.h"

class Player;
class Stage;

class CapturePoint : public CharacterBase3D
{
	friend Stage;

public:
	static constexpr float INTERACT_TIME = 1.0f;

	// コンストラクタ
	CapturePoint(Stage* ptr);

	// 描画処理
	void Draw() override;
	// UI描画処理
	void DrawUI() override;

	// 起動する
	void Activate();
	// 起動中かどうか
	bool IsActive() const { return activeTimer_ > 0.0f; }

private:
	// 拡大率
	static constexpr float SCALE = 0.3f;

	static constexpr float ACTIVE_TIME_MIN = 120.0f;
	static constexpr float ACTIVE_TIME_MAX = 210.0f;

	static constexpr unsigned int COLOR_STANDBY = 0x808080u;
	static constexpr unsigned int COLOR_ACTIVE = 0xffff00u;

	Stage& stage_;
	float activeTimer_ = 0.0f;
	float interactTimer_ = 0.0f;
	bool inRange_ = false;

	// パラメータ初期化
	void InitParam() override;
	// モデル初期化
	void InitModel() override;
	// アニメ初期化
	void InitAnim() override;

	// 移動処理
	void Move() override;

};