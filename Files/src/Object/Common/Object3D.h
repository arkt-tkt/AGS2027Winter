#pragma once
#include <memory>
#include <list>
#include "../../Common/Geometry.h"
#include "Collider3D.h"

class Object3D
{
public:
	// 3Dモデル用ハンドルID
	int handleId;

	// 拡縮
	Vector3 scale;
	// 座標
	Position3 position;
	// 2次座標
	Position3 position2nd;
	// ローカル座標
	Position3 localPosition;
	// 回転
	Vector3 rotation;
	// ローカル回転
	Vector3 localRotation;

	// 自分自身の衝突判定
	std::shared_ptr<Collider3D> collider;

	// コンストラクタ
	Object3D();
	// コンストラクタ(3Dモデルと紐付)
	Object3D(int model_id);
	// デストラクタ
	virtual ~Object3D();

	virtual void Init() {}
	// 拡縮・座標・回転を更新
	void Update();
	// 描画
	void Draw();
	// 解放
	void Release();

private:

	Matrix4x4 mScale;
	Matrix4x4 mRot;
	Matrix4x4 mPos;

	Quaternion qRot;
	Quaternion qLocalRot;

};
