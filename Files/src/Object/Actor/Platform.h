#pragma once
#include "ActorBase3D.h"

class Platform : public ActorBase3D
{
public:
	Platform();
	virtual ~Platform();

	void Update() override;
	void Draw() override;

private:
	// 透過させるフレームの番号
	static constexpr int INVIS_FRAME_ARR[] = {
		468, 493
	};

	// スカイドーム
	Transform3D skyDome_;

	// パラメータ初期化
	void InitParam() override;
	// モデル初期化
	void InitModel() override;
	// アニメ初期化
	void InitAnim() override;

	void Collision() override;

};