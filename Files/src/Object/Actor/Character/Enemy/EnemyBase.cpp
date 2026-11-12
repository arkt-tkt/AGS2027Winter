#include "../../../../Algorithm/PathFind.h"
#include "../../../../Common/DxLibUtil.h"
#include "../../../../Manager/EnemyManager.h"
#include "../../../../Manager/SceneManager.h"
#include "../../../../Utility/RNG.h"
#include "../../../../Application.h"
#include "../Player.h"
#include "EnemyBase.h"

EnemyBase::EnemyBase(EnemyManager* manager)
	:
	enmMng_(*manager)
{
	isAlive_ = false;
	hp_ = 0.0f;
}

EnemyBase::~EnemyBase()
{
}

void EnemyBase::Draw()
{
	if (!isAlive_) return;

#ifdef _DEBUG
	DrawLine3D(
		Vec3ToVEC(transform_.position + lineCollider_.first),
		Vec3ToVEC(transform_.position + lineCollider_.second),
		0xff8000u);

	auto vecZ = Quaternion().Euler(transform_.rotation) * Vector3(0, 0, 10);

	DrawLine3D(
		Vec3ToVEC(transform_.position),
		Vec3ToVEC(transform_.position + vecZ),
		0x0000ffu);
#endif
	
	CharacterBase3D::Draw();
}

void EnemyBase::DrawUI()
{
}

void EnemyBase::MemoryPos(const Position3& pos)
{
	chasePos_ = pos;
}

void EnemyBase::InitParam()
{
	isAlive_ = true;
	maxHp_ = hp_ = 150;
	maxSpeed_ *= 1.4f;
	fullSpeedParallel_ = true;
}

void EnemyBase::InitModel()
{
	Position3 pos;
	pos.x = Application::GetInstance().GetRNGPointer().GetRandReal(-1600.0f, 1600.0f);
	pos.y = 800.0f;
	pos.z = Application::GetInstance().GetRNGPointer().GetRandReal(-1600.0f, 1600.0f);

	transform_.modelId = resMng_.LoadModelDuplicate(ResourceManager::SRC::MODEL_ENEMY1);
	transform_.scale = SCALE;
	transform_.position = pos;
	transform_.localRotation.y = DX_PI_F;

	colliders_.emplace_back(
		std::make_shared<Collider3D>(
			Collider3D::TYPE::COLLIDER,
			Vector3(0, 24, 0),
			Vector3(0, 60, 0),
			8.0f));

	colliders_.emplace_back(
		std::make_shared<Collider3D>(
			Collider3D::TYPE::ENEMY,
			transform_.modelId));

	lineCollider_ = { Vector3(0, -4, 0), Vector3(0, 68, 0) };
}

void EnemyBase::InitAnim()
{
	animation_ = std::make_unique<AnimationController>(transform_.modelId);

	animation_->AddFromOther(ResourceManager::SRC::ANIME_PL_IDLE, 0, 30.0f);

	animation_->Play(ResourceManager::SRC::ANIME_PL_IDLE, 0);
}

void EnemyBase::InitOther()
{
}

void EnemyBase::Move()
{
	if (hitWall_)
	{
		hitWall_ = false;
		Jump();
	}

	transform_.position += velocity_;
}

void EnemyBase::LateMove()
{
	if (!hitWall_)
	{
		// 経路探索
		auto pf = enmMng_.GetPathFindPtr().lock();

		// プレイヤー
		auto player = enmMng_.GetPlayerPtr();
		auto playerPos = player->GetTransform().position;

		// LoSチェック
		bool chkLos = CheckLOS(player->GetTransform().modelId);
		// 方向チェック
		auto qRotVec = (Quaternion().Euler(transform_.rotation) * Vector3(0, 0, 1)).Normalized();
		float chkDir = Dot(qRotVec, (playerPos - transform_.position).Normalized());

		// 追跡状態更新
		chasing_ = chkLos && chkDir >= 0.0f;

		Vector2 moveDir;

		if (chasing_)
		{
			moveDir = (playerPos - transform_.position).XZ().Normalized();

			chasePos_ = playerPos;
		}
		else
		{
			auto path = pf->PathGet(transform_.position, chasePos_);

			if (path.size() > 1)
			{
				unsigned int tgtNode = path.front();

				if (path.front() == lastNode_)
				{
					tgtNode = path[1];
				}

				moveDir = (pf->NodeToPos(tgtNode) - transform_.position).XZ().Normalized();

				if ((pf->NodeToPos(path.front()) - transform_.position).XZ().LengthSquare() <= 10.0f * 10.0f)
				{
					lastNode_ = path.front();
				}
			}
			else
			{
				moveDir = (chasePos_ - transform_.position).XZ().Normalized();
			}
		}

		moveDir *= maxSpeed_ * scnMng_.GetDeltaTime();

		velocity_.x = moveDir.x;
		velocity_.z = moveDir.y;
	}

	AttackPlayer();
}

void EnemyBase::AnimationSwitch()
{
}

void EnemyBase::CollisionMove()
{
	hitWall_ = true;
}

void EnemyBase::TurnVisual()
{
	auto player = enmMng_.GetPlayerPtr();
	Vector3 moveDir = velocity_.Normalized();
	moveDir.y = 0.0f;

	Quaternion qRot = LookRotation(moveDir);

	transform_.rotation.y = qRot.ToEuler().y;

	transform_.Update();
}

void EnemyBase::AttackPlayer()
{
	if ((enmMng_.GetPlayerPtr()->GetTransform().position - transform_.position).LengthSquare() <= 5.0f * 5.0f)
	{
		enmMng_.GetPlayerPtr()->CalcHP(-damage_ * scnMng_.GetDeltaTime());
	}
}
