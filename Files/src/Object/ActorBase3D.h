#pragma once
#include <memory>
#include <vector>
#include "Common/Animation.h"
#include "Common/Object3D.h"

class ResourceManager;
class SceneManager;

class ActorBase3D
{
public:
	static constexpr float GRAVITY_POW = 50.0f;
	static constexpr float JUMP_POW = 7.0f;

	// コンストラクタ
	ActorBase3D(bool alive = true);
	// デストラクタ
	virtual ~ActorBase3D();

	// 初期化処理
	void Init();
	// 更新処理
	void Update();
	// 描画処理
	virtual void Draw() = 0;
	// 解放処理
	void Release();

	// HPを計算
	virtual void CalcHP(float add);
	// 衝突判定を取る相手を追加
	void AddAwayCollider(std::weak_ptr<Collider3D> col);

	// Object3Dを取得
	const Object3D& GetObject3D() const { return object3D_; }
	// 生存フラグを取得
	virtual const bool& IsAlive() const { return isAlive_; }
	// HPを取得
	float GetHP() const { return hp_; }
	// 無敵状態を取得
	bool IsInvincible() const { return invincible_ < 0.0f; }
	// スコアの値を取得
	const int GetScoreValue() const { return scoreValue_; }

protected:
	// シングルトン参照
	ResourceManager& resMng_;
	// シングルトン参照
	SceneManager& scnMng_;

	// オブジェクトの基本情報
	Object3D object3D_;
	// アニメータ
	std::unique_ptr<Animation> anim_;
	// 地面用線分コライダ
	std::pair<Vector3, Vector3> lineCollider_;
	// 衝突判定を取るコライダ
	std::vector<std::weak_ptr<Collider3D>> awayColliders_;

	// 生存フラグ
	bool isAlive_ = true;
	// HP
	float hp_;
	// 最大HP
	float maxHp_;
	// 移動速度
	float maxSpeed_ = 5.0f;
	// 加減速力
	float accel_ = 16.0f;
	// 無敵状態タイマー
	float invincible_ = 0.0f;
	// スコア
	int scoreValue_ = 0;

	// 移動量
	Vector3 velocity_;
	// 着地状態
	bool isLanding_ = true;
	// ダッシュ状態
	bool isSprinting_ = false;

	// モデル初期化
	virtual void InitModel() = 0;
	// アニメ初期化
	virtual void InitAnim() = 0;

	// 移動処理
	virtual void Move();

	void Jump();

	void CollisionCapsule();
	// 重力処理
	void CollisionGravity();

};

