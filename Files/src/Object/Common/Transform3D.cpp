#include <DxLib.h>
#include "../../Common/DxLibUtil.h"
#include "Transform3D.h"

Transform3D::Transform3D()
{
	modelId = -1;

	scale = Vector3(1.0f, 1.0f, 1.0f);
	position = Position3();
	localPosition = Position3();
	rotation = Vector3();
	localRotation = Vector3();

	Init();
}

Transform3D::Transform3D(int model_id)
{
	modelId = model_id;

	scale = Vector3(1.0f, 1.0f, 1.0f);
	position = Position3();
	localPosition = Position3();
	rotation = Vector3();
	localRotation = Vector3();

	Init();
}

Transform3D::~Transform3D()
{
	MV1DeleteModel(modelId);
}

void Transform3D::Update()
{
	mScale = ScaleMatrix(scale);

	Matrix4x4 mp = TranslationMatrix(position);
	Matrix4x4 mip = TranslationMatrix(-position);

	qLocalRot.Euler(localRotation);
	qRot.Euler(rotation);

	mRot = (qRot * qLocalRot).ToMatrix();

	// ローカル座標を原点とした回転を行った場合の座標を得る
	mPos = TranslationMatrix((mp * mRot * mip) * (localPosition + position));

	Matrix4x4 mat = mScale * mRot % mPos;

	if (modelId != -1)
	{
		MV1SetMatrix(modelId, DxLibUtil::Mat4x4ToMAT(mat));
	}
}

void Transform3D::Draw()
{
	MV1DrawModel(modelId);
}

void Transform3D::Release()
{
	position = position2nd = localPosition = {};
	rotation = localRotation = {};
}