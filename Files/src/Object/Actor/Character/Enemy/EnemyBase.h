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

	void MemoryPos(const Position3& pos);

	float GetDamage() const { return damage_; }

private:
	static constexpr float SCALE = 0.4f;

	// マネージャ
	EnemyManager& enmMng_;

	// 攻撃で与えるダメージ量
	float damage_ = 20.0f;
	// 壁に当たったか
	bool hitWall_ = false;

	// 追跡状態
	bool chasing_ = false;
	// 最後にプレイヤーの痕跡を見つけた場所
	Position3 chasePos_;
	// 最後に通過したノード
	unsigned int lastNode_ = (unsigned int)(-1);

	void InitParam() override;
	void InitModel() override;
	void InitAnim() override;
	void InitOther() override;

	void Move() override;
	void LateMove() override;
	void AnimationSwitch() override;

	void CollisionMove() override;

	void TurnVisual() override;

	void AttackPlayer();

};
