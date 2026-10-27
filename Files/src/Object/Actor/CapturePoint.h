#pragma once
#include "ActorBase3D.h"

class CapturePoint : public ActorBase3D
{
public:
	// 更新処理
	void Update() override;
	// UI描画処理
	void DrawUI() override;

private:
	// パラメータ初期化
	void InitParam() override;
	// モデル初期化
	void InitModel() override;
	// アニメ初期化
	void InitAnim() override;

};