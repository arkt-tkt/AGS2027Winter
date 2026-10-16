#pragma once
#include <memory>
#include <vector>
#include "../Common/Transform3D.h"

class ResourceManager;
class SceneManager;

class ActorBase3D
{
public:
	// コンストラクタ
	ActorBase3D();
	// デストラクタ
	virtual ~ActorBase3D();

	// 初期化処理
	void Init();
	// 更新処理
	virtual void Update() = 0;
	// 描画処理
	virtual void Draw() = 0;
	// 解放処理
	void Release();

	// 衝突判定を取る相手を追加
	void AddAwayCollider(std::weak_ptr<Collider3D> col);
	// 衝突判定を取る相手を追加
	void AddAwayColliders(std::vector<std::shared_ptr<Collider3D>> cols);

	// Object3Dを取得
	const Transform3D& GetTransform() const { return transform_; }

	std::vector<std::shared_ptr<Collider3D>> GetTypedColliders(Collider3D::TYPE type) const;

protected:
	// シングルトン参照
	ResourceManager& resMng_;
	// シングルトン参照
	SceneManager& scnMng_;

	// オブジェクトの基本情報
	Transform3D transform_;
	// 自分自身の衝突判定
	std::vector<std::shared_ptr<Collider3D>> colliders_;
	// 衝突判定を取るコライダ
	std::vector<std::weak_ptr<Collider3D>> awayColliders_;

	// 移動量
	Vector3 velocity_;

	// モデル初期化
	virtual void InitModel() = 0;
	// アニメ初期化
	virtual void InitAnim() = 0;

	// 移動処理
	virtual void Move() {};

	virtual void Collision() {};

};

