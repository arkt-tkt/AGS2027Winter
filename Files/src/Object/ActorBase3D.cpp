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

void ActorBase3D::AddAwayCollider(std::weak_ptr<Collider3D> col)
{
	awayColliders_.push_back(col);
}

void ActorBase3D::AddAwayColliders(std::vector<std::shared_ptr<Collider3D>> cols)
{
	for (auto& col : cols)
	{
		awayColliders_.push_back(col);
	}
}

void ActorBase3D::Move()
{
}

void ActorBase3D::Jump()
{
	if (!isLanding_) return;

	object3D_.position.y += JUMP_OFFSET;
	velocity_.y = JUMP_POW;
	isLanding_ = false;
}

void ActorBase3D::CollisionCapsule()
{
	for (const auto& c : awayColliders_)
	{
		auto& awColDat = c.lock()->GetColliderData();

		if (awColDat.type != Collider3D::TYPE::STAGE || awColDat.handleId == -1) continue;

		// コレに自分のコライダが入る
		Collider3D* col = nullptr;

		// カプセルコライダ探し
		for (auto& collider : object3D_.colliders)
		{
			if (collider->GetColliderData().type == Collider3D::TYPE::COLLIDER)
			{
				col = collider.get();
			}
		}

		// nullptrのままだと、恐らくobject3D_.collidersの中身が不足しているので返す
		if (!col) return;

		auto hits = MV1CollCheck_Capsule(
			c.lock()->GetColliderData().handleId, -1,
			Vec3ToVEC(col->GetColliderData().pos1),
			Vec3ToVEC(col->GetColliderData().pos2),
			col->GetColliderData().r);

		for (int i = 0; i < hits.HitNum; i++)
		{
			auto hit = hits.Dim[i];

			// 衝突判定後１度だけ行う分岐
			if (VDot(hit.Normal, VGet(0, -1, 0)) >= 0.8f || // 衝突対象が天井と思しき場合
				VDot(hit.Normal, VGet(0, 1, 0)) >= 0.8f || // 衝突対象が床と思しき場合
				VDot(hit.Normal, Vec3ToVEC(velocity_)) >= 0.5f) // 衝突対象の法線が移動方向と近しい場合
			{
				continue;
			}

			// 衝突しないギリギリになるまで繰り返す
			for (int tryCnt = 0; tryCnt < 50; tryCnt++)
			{
				int pHit = HitCheck_Capsule_Triangle(
					Vec3ToVEC(col->GetColliderData().pos1),
					Vec3ToVEC(col->GetColliderData().pos2),
					col->GetColliderData().r,
					hit.Position[0], hit.Position[1], hit.Position[2]);

				if (pHit)
				{
					// ほんの少しずつ法線方向にズラす
					object3D_.position += VECToVec3(hit.Normal) * 0.05f;
					object3D_.Update();
					continue;
				}

				// 移動方向と法線のズレに反比例して最大移動速度を決定
				auto vec = (std::min)(Dot(velocity_, VECToVec3(hit.Normal)) + 1.0f, 1.0f);
				velocity_.x *= vec;
				velocity_.z *= vec;

				// 移動方向を法線と平行にする
				velocity_ = Parallel(velocity_, VECToVec3(hit.Normal));

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

		if (hits.HitNum)
		{
			bool compHit = false;
			auto retPos = Vector3(0, 1000, 0);
			auto basePos = object3D_.position + Vector3(0, 20, 0);

			for (int i = 0; i < hits.HitNum; ++i)
			{
				auto hit = hits.Dim[i];

				// 衝突対象が天井と思しき場合
				if (VDot(hit.Normal, VGet(0, -1, 0)) >= 0.5f)
				{
					if (!isLanding_ && velocity_.y > 0.0f)
					{
						velocity_.y = 0.0f;
					}
					continue;
				}

				auto hitPos = VECToVec3(hit.HitPosition);

				if (basePos.y < hitPos.y)
				{
					continue;
				}

				if ((hitPos - basePos).LengthSquare() < (retPos - basePos).LengthSquare())
				{
					retPos = hitPos;
					compHit = true;
				}
			}

			if (compHit)
			{
				object3D_.position.y = retPos.y;
				velocity_.y = 0.0f;
				isLanding_ = true;
			}
			else
			{
				velocity_.y = (std::max)(velocity_.y - GRAVITY_POW * SceneManager::GetInstance().GetDeltaTime(), MAX_FALL_VELOCITY);
				isLanding_ = false;
			}
		}
		else
		{
			velocity_.y = (std::max)(velocity_.y - GRAVITY_POW * SceneManager::GetInstance().GetDeltaTime(), MAX_FALL_VELOCITY);
			isLanding_ = false;
		}

		// 検出した地面ポリゴン情報の後始末
		MV1CollResultPolyDimTerminate(hits);
	}
}
