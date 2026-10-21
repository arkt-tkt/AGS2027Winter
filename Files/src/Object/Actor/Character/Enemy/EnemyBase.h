#pragma once
#include "../CharacterBase3D.h"

class EnemyBase : public CharacterBase3D
{
public:
	EnemyBase();
	virtual ~EnemyBase();

	void Draw() override;
	void DrawUI() override;

	void Spawn();

private:
	static constexpr float SCALE = 0.4f;

	void InitModel() override;
	void InitAnim() override;
	void InitOther() override;

	void Move() override;

	void AnimationSwitch() override;

	void TurnBody();

};
