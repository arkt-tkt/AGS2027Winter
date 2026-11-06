#pragma once
#include "../Character/CharacterBase3D.h"

class Player;
class Stage;

class Resupply : public CharacterBase3D
{
	friend Stage;

public:
	static constexpr float INTERACT_TIME = 1.0f;

	// コンストラクタ
	Resupply(Stage* ptr);
	// デストラクタ
	~Resupply();

	// 描画処理
	void Draw() override;
	// UI描画処理
	void DrawUI() override;

	// 起動する
	void Activate();

private:
	// 拡大率
	static constexpr float SCALE = 0.8f;

	Stage& stage_;

	float activeTimer_ = 0.0f;
	float interactTimer_ = 0.0f;
	bool inRange_ = false;

	// パラメータ初期化
	void InitParam() override {}
	// モデル初期化
	void InitModel() override;
	// アニメ初期化
	void InitAnim() override {}

	// 移動処理
	void Move() override;

};