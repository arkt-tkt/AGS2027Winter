#include <DxLib.h>
#include <utility>
#include "../../Common/DxLibUtil.h"
#include "Collider3D.h"

Collider3D::Collider3D(TYPE type, const Position3& local_pos, float r)
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

Collider3D::Collider3D(TYPE type, const Position3& local_pos1, const Position3& local_pos2)
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

Collider3D::Collider3D(TYPE type, const Position3& local_pos1, const Position3& local_pos2, float r)
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
	MV1SetupCollInfo(baseData_.modelId);
	SharedConstructor();
}

void Collider3D::Update(const Position3& world_pos, const Vector3& world_rot)
{
	if (data_.shape == SHAPE::CAPSULE_ALT) return;

	if (baseData_.shape == SHAPE::MODEL)
	{
		MV1RefreshCollInfo(baseData_.modelId);
		return;
	}

	data_ = baseData_;

	Quaternion qRot;
	qRot.Euler(world_rot);
	Matrix4x4 mRot = TransposeMatrix(qRot.ToMatrix());

	switch (baseData_.shape)
	{
	case SHAPE::SPHERE:
		data_.pos1 = (mRot * baseData_.pos1) + world_pos;
		break;
	case SHAPE::LINE: case SHAPE::CAPSULE:
		data_.pos1 = (mRot * baseData_.pos1) + world_pos;
		data_.pos2 = (mRot * baseData_.pos2) + world_pos;
		break;
	}
}

void Collider3D::UpdateAlt(const Position3& world_pos1, const Position3& world_pos2)
{
	if (data_.shape != SHAPE::CAPSULE_ALT) return;

	data_ = baseData_;

	data_.pos1 = world_pos1;
	data_.pos2 = world_pos2;
}

void Collider3D::Release()
{
	if (baseData_.shape == SHAPE::MODEL &&
		baseData_.modelId != -1)
	{
		MV1TerminateCollInfo(baseData_.modelId);
	}
}

#ifdef _DEBUG
void Collider3D::DrawDebug() const
{
	switch (data_.shape)
	{
	case SHAPE::SPHERE:
		DrawSphere3D(
			DxLibUtil::Vec3ToVEC(data_.pos1),
			data_.r,
			COLLIDER_DIV,
			COLLIDER_COLOR,
			COLLIDER_COLOR,
			FALSE);
		break;
	case SHAPE::LINE:
		DrawLine3D(
			DxLibUtil::Vec3ToVEC(data_.pos1),
			DxLibUtil::Vec3ToVEC(data_.pos2),
			COLLIDER_COLOR);
		break;
	case SHAPE::CAPSULE: case SHAPE::CAPSULE_ALT:
		DrawCapsule3D(
			DxLibUtil::Vec3ToVEC(data_.pos1),
			DxLibUtil::Vec3ToVEC(data_.pos2),
			data_.r,
			COLLIDER_DIV,
			COLLIDER_COLOR,
			COLLIDER_COLOR,
			FALSE);
		break;
	}
}
#endif

MV1_COLL_RESULT_POLY Collider3D::GetNearestHitPolyLine(const Position3& start, const Position3& end)
{
	MV1_COLL_RESULT_POLY ret = {};

	// ëÅä˙return
	if (baseData_.shape != SHAPE::MODEL) return ret;

	// ê¸ï™Ç≈è’ìÀîªíË
	auto hits = MV1CollCheck_LineDim(data_.modelId, -1,
		Vec3ToVEC(end), Vec3ToVEC(start));

	// í«è]ëŒè€Ç…àÍî‘ãﬂÇ¢è’ìÀì_ÇíTÇ∑
	bool isCollision = false;
	MV1_COLL_RESULT_POLY hitPoly = {};
	double minDist = DBL_MAX;
	for (int i = 0; i < hits.HitNum; i++)
	{
		const auto& hit = hits.Dim[i];
		
		// è’ìÀîªíË
		isCollision = true;

		// ãóó£îªíË
		auto dist = (double)(VECToVec3(hit.HitPosition) - start).LengthSquare();
		if (dist < minDist)
		{
			// í«è]ëŒè€Ç…àÍî‘ãﬂÇ¢è’ìÀì_ÇóDêÊ
			minDist = dist;
			ret = hit;
		}
	}

	// åüèoÇµÇΩÉ|ÉäÉSÉìèÓïÒÇÃå„énññ
	MV1CollResultPolyDimTerminate(hits);

	return ret;
}

void Collider3D::SharedConstructor()
{
	data_ = baseData_;
}
