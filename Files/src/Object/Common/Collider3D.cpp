#include <DxLib.h>
#include <utility>
#include "../../Common/DxLibUtil.h"
#include "Collider3D.h"

Collider3D::Collider3D(TYPE type, Position3 local_pos, float r)
	:
	baseData_(
		type,
		SHAPE::SPHERE,
		local_pos,
		Position3(),
		r,
		-1
	)
{
	SharedConstructor();
}

Collider3D::Collider3D(TYPE type, Position3 local_pos1, Position3 local_pos2)
	:
	baseData_(
		type,
		SHAPE::LINE,
		local_pos1,
		local_pos2,
		0.0f,
		-1
	)
{
	SharedConstructor();
}

Collider3D::Collider3D(TYPE type, Position3 local_pos1, Position3 local_pos2, float r)
	:
	baseData_(
		type,
		SHAPE::CAPSULE,
		local_pos1,
		local_pos2,
		r,
		-1
	)
{
	SharedConstructor();
}

Collider3D::Collider3D(TYPE type, float r)
	:
	baseData_(
		type,
		SHAPE::CAPSULE_ALT,
		Position3(),
		Position3(),
		r,
		-1
	)
{
	SharedConstructor();
}

Collider3D::Collider3D(TYPE type, int handle_id)
	:
	baseData_(
		type,
		SHAPE::MODEL,
		Position3(),
		Position3(),
		0.0f,
		handle_id
	)
{
	MV1SetupCollInfo(baseData_.handleId);
	SharedConstructor();
}

Collider3D::~Collider3D()
{
}

void Collider3D::Update(Position3 world_pos, Vector3 world_rot)
{
	data_ = baseData_;

	Quaternion qRot;
	qRot.Euler(world_rot);
	Matrix4x4 mRot = TransposeMatrix(qRot.ToMatrix());

	switch (baseData_.shape)
	{
	case SHAPE::SPHERE:
		data_.pos1 = (mRot * baseData_.pos1) + world_pos;
		break;
	case SHAPE::CAPSULE:
		data_.pos1 = (mRot * baseData_.pos1) + world_pos;
		data_.pos2 = (mRot * baseData_.pos2) + world_pos;
		break;
	case SHAPE::MODEL:
		MV1RefreshCollInfo(baseData_.handleId);
		break;
	}
}

void Collider3D::UpdateAlt(Position3 world_pos1, Vector3 world_pos2)
{
	if (data_.shape != SHAPE::CAPSULE_ALT) return;

	data_ = baseData_;

	data_.pos1 = world_pos1;
	data_.pos2 = world_pos2;
}

void Collider3D::Release()
{
	if (baseData_.handleId != -1)
	{
		MV1TerminateCollInfo(baseData_.handleId);
	}
}

void Collider3D::DrawDebug() const
{
	constexpr unsigned int color = 0xff8000u;

	switch (data_.shape)
	{
	case SHAPE::SPHERE:
		DrawSphere3D(
			DxLibUtil::Vec3ToVEC(data_.pos1),
			data_.r,
			COLLIDER_DIV,
			color,
			color,
			FALSE
		);
		break;
	case SHAPE::LINE:
		DrawLine3D(
			DxLibUtil::Vec3ToVEC(data_.pos1),
			DxLibUtil::Vec3ToVEC(data_.pos2),
			color
		);
		break;
	case SHAPE::CAPSULE: case SHAPE::CAPSULE_ALT:
		DrawCapsule3D(
			DxLibUtil::Vec3ToVEC(data_.pos1),
			DxLibUtil::Vec3ToVEC(data_.pos2),
			data_.r,
			COLLIDER_DIV,
			color,
			color,
			FALSE
		);
	}
}

const Collider3D::COLLIDER_DATA& Collider3D::GetColliderData() const
{
	return data_;
}

void Collider3D::SharedConstructor()
{
	data_ = baseData_;
}
