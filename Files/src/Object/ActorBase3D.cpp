#include "../Common/DxLibUtil.h"
#include "../Manager/FPSManager.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/SceneManager.h"
#include "ActorBase3D.h"

ActorBase3D::ActorBase3D(bool alive)
	:
	resMng_(ResourceManager::GetInstance()),
	scnMng_(SceneManager::GetInstance()),
	isAlive_(alive),
	hp_(alive ? 1.0f : 0.0f)
{
}

ActorBase3D::~ActorBase3D()
{
	Release();
}

void ActorBase3D::Init()
{
	InitModel();

	InitAnim();

	object3D_.Update();
}

void ActorBase3D::Update()
{
	if (anim_) anim_->Update();

	Move();

	object3D_.position += velocity_;

	CollisionCapsule();

	CollisionGravity();

	object3D_.Update();
}

void ActorBase3D::Release()
{
	isAlive_ = false;
	object3D_.Release();
}

const Object3D& ActorBase3D::GetObject3D() const
{
	return object3D_;
}

bool ActorBase3D::IsAlive() const
{
	return isAlive_;
}

float ActorBase3D::GetHP() const
{
	return hp_;
}

void ActorBase3D::CalcHP(float add)
{
	if (IsInvincible() && add < 0.0f) return;

	hp_ += add;
	if (hp_ <= 0.0f)
	{
		hp_ = 0.0f;
		isAlive_ = false;
	}
}

bool ActorBase3D::IsInvincible() const
{
	return invincible_ < 0.0f;
}

const int ActorBase3D::GetScoreValue() const
{
	return scoreValue_;
}

void ActorBase3D::AddAwayCollider(std::weak_ptr<Collider3D> col)
{
	awayColliders_.push_back(col);
}

void ActorBase3D::Move()
{
}

void ActorBase3D::Jump()
{
	if (!isLanding_) return;

	velocity_.y = JUMP_POW;
	isLanding_ = false;
}

void ActorBase3D::CollisionCapsule()
{
	if (!object3D_.collider) return;

	for (const auto& c : awayColliders_)
	{
		auto hits = MV1CollCheck_Capsule(
			c.lock()->GetColliderData().handleId, -1,
			Vec3ToVEC(object3D_.collider->GetColliderData().pos1),
			Vec3ToVEC(object3D_.collider->GetColliderData().pos2),
			object3D_.collider->GetColliderData().r);

		for (int i = 0; i < hits.HitNum; i++)
		{
			auto hit = hits.Dim[i];

			// 法線が天井と思しき場合
			if (VDot(hit.Normal, VGet(0, -1, 0)) >= 0.8f && velocity_.y > 0.0f)
			{
				// 上下移動速度を殺す
				velocity_.y = 0.0f;
			}
			// 法線と自分の移動方向が近しい場合
			else if (Dot(velocity_, VECToVec3(hit.Normal)) >= 0.0f)
			{
				// 薄い壁を抜ける原因になってしまうため、この接触判定は触らない
				continue;
			}

			for (int tryCnt = 0; tryCnt < 40; tryCnt++)
			{
				int pHit = HitCheck_Capsule_Triangle(
					Vec3ToVEC(object3D_.collider->GetColliderData().pos1),
					Vec3ToVEC(object3D_.collider->GetColliderData().pos2),
					object3D_.collider->GetColliderData().r,
					hit.Position[0], hit.Position[1], hit.Position[2]);

				if (pHit)
				{
					object3D_.position += VECToVec3(hit.Normal) * 0.5f;
					object3D_.Update();
					continue;
				}

				break;
			}
		}

		// 検出した地面ポリゴン情報の後始末
		MV1CollResultPolyDimTerminate(hits);
	}
}

void ActorBase3D::CollisionGravity()
{
	for (const auto& c : awayColliders_)
	{
		auto& dat = c.lock()->GetColliderData();

		if (dat.type != Collider3D::TYPE::STAGE) continue;

		Vector3 col1 = object3D_.position + lineCollider_.first;
		Vector3 col2 = object3D_.position + lineCollider_.second;

		auto hits = MV1CollCheck_LineDim(
			dat.handleId, -1,
			Vec3ToVEC(col1),
			Vec3ToVEC(col2));

		if (hits.HitNum && Dot(Vector3(0, velocity_.y, 0), Vector3(0, -1, 0)) > 0.0f)
		{
			auto hitPos = Vector3(0, 1000, 0);
			auto basePos = object3D_.position + Vector3(0, 20, 0);

			for (int i = 0; i < hits.HitNum; ++i)
			{
				auto pos = VECToVec3(hits.Dim[i].HitPosition);

				if ((pos - basePos).LengthSquare() < (hitPos - basePos).LengthSquare())
				{
					hitPos = pos;
				}
			}

			object3D_.position.y = hitPos.y + 0.5f;
			velocity_.y = 0.0f;
			isLanding_ = true;
		}
		else
		{
			velocity_.y -= GRAVITY_POW * SceneManager::GetInstance().GetDeltaTime();
			isLanding_ = false;
		}

		// 検出した地面ポリゴン情報の後始末
		MV1CollResultPolyDimTerminate(hits);
	}
}
