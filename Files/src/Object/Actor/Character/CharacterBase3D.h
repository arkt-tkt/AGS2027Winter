#pragma once
#include "../../Common/Animation.h"
#include "../ActorBase3D.h"

class CharacterBase3D : public ActorBase3D
{
public:
	static constexpr float JUMP_OFFSET = 3.0f;
	static constexpr float JUMP_POW = 6.0f;
	static constexpr float GRAVITY_POW = 32.0f;
	static constexpr float MAX_FALL_VELOCITY = -24.0f;

	CharacterBase3D() : ActorBase3D() {}
	virtual ~CharacterBase3D() {}

	void Update() override final;
	void LateUpdate() override final;

	void Draw() override;
	virtual void DrawUI() {}

	// 生存フラグを取得
	virtual const bool& IsAlive() const { return isAlive_; }
	// HPを取得
	float GetHP() const { return hp_; }
	// HPを計算
	virtual void CalcHP(float add);
	// 無敵状態を取得
	bool IsInvincible() const { return invincible_ < 0.0f; }
	// スコアの値を取得
	const int GetScoreValue() const { return scoreValue_; }

protected:
	// アニメータ
	std::unique_ptr<Animation> animation_;
	// 地面用線分コライダ
	std::pair<Vector3, Vector3> lineCollider_;

	// 生存フラグ
	bool isAlive_ = true;
	// HP
	float hp_ = 100.0f;
	// 最大HP
	float maxHp_ = hp_;
	// 1秒あたりの最大移動速度
	float maxSpeed_ = 120.0f;
	// ダッシュ倍率
	float sprintMult_ = 1.5f;
	// 加速力
	float accel_ = 12.0f;
	// 減速力
	float deccel_ = 9.0f;
	// 無敵状態タイマー
	float invincible_ = 0.0f;
	// スコア
	int scoreValue_ = 0;

	// 着地状態
	bool isLanding_ = true;
	// ダッシュ状態
	bool isSprinting_ = false;

	// ジャンプ入力への反応
	void Jump();
	
	// アニメーション変更
	virtual void AnimationSwitch() {}

	// カプセルコライダの衝突判定
	bool CollisionCapsule();
	// 重力方向の衝突判定
	void CollisionGravity();
	// 衝突時のみ発生する移動処理
	virtual void CollisionMove() {}

	// その他下位クラスの処理
	virtual void SubClass() {}
};
