#include <DxLib.h>
#include "../../Common/DxLibUtil.h"
#include "Object3D.h"

Object3D::Object3D()
{
	handleId = -1;

	scale = Vector3(1.0f, 1.0f, 1.0f);
	position = Position3();
	localPosition = Position3();
	rotation = Vector3();
	localRotation = Vector3();

	Init();
}

Object3D::Object3D(int model_id)
{
	handleId = model_id;

	scale = Vector3(1.0f, 1.0f, 1.0f);
	position = Position3();
	localPosition = Position3();
	rotation = Vector3();
	localRotation = Vector3();

	Init();
}

Object3D::~Object3D()
{
}

void Object3D::Update()
{
	mScale = ScaleMatrix(scale);

	Matrix4x4 mp = TranslationMatrix(position);
	Matrix4x4 mip = TranslationMatrix(-position);

	qLocalRot.Euler(localRotation);
	qRot.Euler(rotation);

	mRot = (qLocalRot * qRot).ToMatrix();

	// ローカル座標を原点とした回転を行った場合の座標を得る
	mPos = TranslationMatrix((mp * mRot * mip) * (localPosition + position));

	Matrix4x4 mat = mScale * mRot % mPos;

	if (handleId != -1)
	{
		MV1SetMatrix(handleId, DxLibUtil::Mat4x4ToMAT(mat));
	}

	// Collider3Dの座標情報更新
	if (!collider) return;

	if (collider->GetColliderData().shape != Collider3D::SHAPE::CAPSULE_ALT)
	{
		collider->Update(position, rotation);
	}
	else
	{
		collider->UpdateAlt(position, position2nd);
	}

}

void Object3D::Draw()
{
	MV1DrawModel(handleId);

#ifdef _DEBUG
	// Collider3Dの描画
	if (!collider) return;

	collider->DrawDebug();
#endif
}

void Object3D::Release()
{
	position = position2nd = localPosition = {};
	rotation = localRotation = {};

	if (collider) collider->Release();
}