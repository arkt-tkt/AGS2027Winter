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

	void GetSoundPos(const Position3& pos);

	float GetDamage() const { return damage_; }

private:
	static constexpr float SCALE = 0.4f;
	static constexpr float CHASE_UPDATE_SOUND_LEN_SPACE = 25.0f;
	static constexpr float TARGET_NEAR = 38.0f;
	static constexpr float ATTACK_RANGE = 200.0f;// 42.0f;
	static constexpr float ATTACK_HIT_RADIUS = 3.0f;

	// マネージャ
	EnemyManager& enmMng_;

	// 攻撃で与えるダメージ量
	float damage_ = 15.0f;
	// 壁に当たったか
	bool hitWall_ = false;

	// 追跡状態
	bool chasing_ = false;
	// 直前フレームの追跡状態
	bool prevChasing_ = chasing_;
	// 最後に知覚情報で得られた行先
	Position3 chasePos_;
	// 経路探索のパス
	std::vector<int> pathData_;
	// 最後に通過したノード
	unsigned int lastNode_ = (unsigned int)(-1);
	// 攻撃が当たる状態
	bool isCanHit_ = false;

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
