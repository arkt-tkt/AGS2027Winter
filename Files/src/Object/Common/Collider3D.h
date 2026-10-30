#pragma once
#include <DxLib.h>
#include <vector>
#include "../../Common/Geometry.h"

class Collider3D
{
public:
	enum class TYPE
	{
		COLLIDER,			// コライダ(対ステージ用)
		PLAYER,				// プレイヤー
		ENEMY,				// 敵
		PROJECTILE_PLAYER,	// プレイヤー側の弾
		PROJECTILE_ENEMY,	// 敵側の弾
		STAGE,				// ステージ
	};

	enum class SHAPE
	{
		SPHERE,			// 球
		LINE,			// 線分
		CAPSULE,		// カプセル
		CAPSULE_ALT,	// カプセル(座標追従)
		MODEL,			// 3Dモデル
	};

	struct COLLIDER_DATA
	{
		TYPE type;
		SHAPE shape;
		Position3 pos1;
		Position3 pos2;
		float r;
		int modelId;
	};

	// SPHERE コンストラクタ
	Collider3D(TYPE type, const Position3& local_pos, float r);
	// LINE コンストラクタ
	Collider3D(TYPE type, const Position3& local_pos1, const Position3& local_pos2);
	// CAPSULE コンストラクタ
	Collider3D(TYPE type, const Position3& local_pos1, const Position3& local_pos2, float r);
	// CAPSULE_ALT コンストラクタ
	// (座標をTransformから直接取得して更新する。移動の軌跡を追いかけるため発射物に特化)
	Collider3D(TYPE type, float r);
	// MODEL コンストラクタ
	Collider3D(TYPE type, int handle_id);

	// デストラクタ
	virtual ~Collider3D() {}

	// 更新
	void Update(const Position3& world_pos, const Vector3& world_rot);
	// 更新(CAPSULE_ALT専用)、Transform3D側で分岐制御
	void UpdateAlt(const Position3& world_pos1, const Position3& world_pos2);
	// 解放
	void Release();

#ifdef _DEBUG
	// デバッグ用描画
	void DrawDebug() const;
#endif

	// データを取得させる
	const COLLIDER_DATA& GetColliderData() const { return data_; }

	// このコライダに対する線分コライダの、一番近い衝突点を探す
	// (SHAPE::MODEL以外で使っても効果は無いので注意)
	MV1_COLL_RESULT_POLY GetNearestHitPolyLine(const Position3& start, const Position3& end);

private:
#ifdef _DEBUG
	static constexpr int COLLIDER_DIV = 8;
	static constexpr unsigned int COLLIDER_COLOR = 0xff8000u;
#endif

	// 基礎データ
	COLLIDER_DATA baseData_;

	// 外部取得用データ
	// (基礎データに座標や回転情報をブレンドしたもの)
	COLLIDER_DATA data_;

	// 共通コンストラクタ
	void SharedConstructor();

};