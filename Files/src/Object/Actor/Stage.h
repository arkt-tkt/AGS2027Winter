#pragma once
#include "ActorBase3D.h"

class Stage : public ActorBase3D
{
public:
	Stage();
	virtual ~Stage();

	void Update() override;
	void Draw() override;

private:
	static constexpr int INVIS_FRAME_ARR[] = {
		468, 493
	};

	Transform3D skyDome_;

	// パラメータ初期化
	void InitParam() override;
	// モデル初期化
	void InitModel() override;
	// アニメ初期化
	void InitAnim() override;

	void Collision() override;

};