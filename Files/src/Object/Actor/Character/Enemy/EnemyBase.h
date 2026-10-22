#pragma once
#include "../CharacterBase3D.h"

class EnemyManager;

class EnemyBase : public CharacterBase3D
{
public:
	enum class TYPE
	{
		RUSHER,
	};

	EnemyBase(EnemyManager* manager);
	virtual ~EnemyBase();

	void Draw() override;
	void DrawUI() override;

	float GetDamage() const { return damage_; }

private:
	static constexpr float SCALE = 0.4f;

	EnemyManager& enmMng_;
	float damage_ = 5.0f;

	void InitParam() override;
	void InitModel() override;
	void InitAnim() override;
	void InitOther() override;

	void Move() override;
	void AnimationSwitch() override;

	void CollisionMove() override;

	void TurnVisual() override;

};
