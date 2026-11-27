#pragma once
#include "../../Common/AnimationController.h"
#include "../ActorBase3D.h"

class CharacterBase3D : public ActorBase3D
{
public:
	static constexpr float JUMP_OFFSET = 3.0f;
	static constexpr float JUMP_POW = 6.0f;
	static constexpr float GRAVITY_POW = 32.0f;
	static constexpr float MAX_FALL_VELOCITY = -24.0f;
	static constexpr float SPRINT_STANCE_RATE = 4.0f;

	static constexpr float DAMAGE_TIMER_MAX = 0.5f;

	struct Stats
	{
		int model = 0;
		Vector3 scale = { 1.0f, 1.0f, 1.0f };
		int bounty = 0;
		float hp = 100;
		float regen = 1.0f;
		float shield = 0;
		float recharge = 0.0f;
		float armor = 0;
		float damage = 5;
		float attackSpeed = 1.0f;
		float movement = 120;
		float sprint = 1.8f;
		float accel = 9.0f;
		float deccel = 9.0f;
	};

	CharacterBase3D() : ActorBase3D() {}

	void Update() override final;
	void LateUpdate() override final;

	void Draw() override;

	// 生存フラグを取得
	virtual const bool& IsAlive() const { return isAlive_; }
	// HPを取得
	float GetHP() const { return hp_; }
	// 最大HPを取得
	float GetMaxHP() const { return baseStats_.hp; }
	// HPを計算
	virtual void CalcHP(float add);
	// ダメージタイマを取得
	float GetDamageTimer() const { return damageTimer_; }
	// 無敵状態を取得
	bool IsInvincible() const { return invincible_ < 0.0f; }
	// スコアの値を取得
	const int GetScoreValue() const { return scoreValue_; }

protected:
	// アニメータ
	std::unique_ptr<AnimationController> animation_;
	// 地面用線分コライダ
	std::pair<Vector3, Vector3> lineCollider_;

	// 基礎ステータス
	Stats baseStats_;
	// 生存フラグ
	bool isAlive_ = true;

	// HP
	float hp_ = 100.0f;
	// ダメージを受けると作動するタイマ
	float damageTimer_ = 0.0f;

	// 無敵状態タイマー
	float invincible_ = 0.0f;
	// スコア
	int scoreValue_ = 0;

	// ローカル移動方向
	Vector3 moveDir_;

	// 着地状態
	bool isLanding_ = true;
	// ダッシュ状態値
	float sprintRate_ = 0.0f;
	// 衝突時の対法線平行移動速度をフルスピードで可能にするかどうか
	bool fullSpeedParallel_ = false;

	// ジャンプ入力への反応
	void Jump();
	
	// アニメーション変更
	virtual void AnimationSwitch() {}

	// ステージに対するカプセルコライダの衝突判定
	bool CollisionStage();
	// CollisionStage() = trueの場合のみ発生する移動処理
	virtual void CollisionMove() {}
	// 重力方向の衝突判定
	void CollisionGravity();
	// 発射物の衝突判定
	void CollisionProjectile();

	// 視線判定
	bool CheckLOS(int away_model_id);

	// その他下位クラスの処理
	virtual void LateMove() {}
};
