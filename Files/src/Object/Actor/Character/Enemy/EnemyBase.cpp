#include "../../../../Algorithm/PathFind.h"
#include "../../../../Common/DxLibUtil.h"
#include "../../../../Common/MathUtil.h"
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

void EnemyBase::GetSoundPos(const Position3& pos)
{
	if ((pos - chasePos_).LengthSquare() >
		CHASE_UPDATE_SOUND_LEN_SPACE * CHASE_UPDATE_SOUND_LEN_SPACE)
	{
		chasePos_ = pos;

		pathData_ = enmMng_.GetPathFindPtr().lock()->
			PathGetR(transform_.position, chasePos_);
	}
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

	float rot = Application::GetInstance().GetRNGPointer().GetRandReal(0.0f, 2.0f * MathUtil::PI_F);

	transform_.modelId = resMng_.LoadModelDuplicate(ResourceManager::SRC::MODEL_ENEMY1);
	transform_.scale = SCALE;
	transform_.position = pos;
	transform_.rotation.y = rot;
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

	animation_->AddFromOther(ResourceManager::SRC::ANIME_EN_IDLE, 0, 30.0f);
	animation_->AddFromOther(ResourceManager::SRC::ANIME_EN_MOVE, 0, 30.0f,
		AnimationController::PARTS::FULL_BODY);
	animation_->AddFromOther(ResourceManager::SRC::ANIME_EN_PUNCH, 0, 30.0f,
		AnimationController::PARTS::UPPER_BODY);

	animation_->Play(ResourceManager::SRC::ANIME_EN_IDLE, 0);
}

void EnemyBase::InitOther()
{
	chasePos_ = transform_.position;
	chasePos_.y = 0.0f;

	pathData_.reserve(10);
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
		prevChasing_ = chasing_;
		chasing_ = chkLos && chkDir >= 0.0f;

		Vector2 moveDir;

		// 追跡および経路探索
		if (chasing_)
		{
			auto vec = (playerPos - transform_.position).XZ();

			if (vec.LengthSquare() > TARGET_NEAR * TARGET_NEAR)
			{
				// 追跡中は直接プレイヤーを追いかける
				moveDir = vec.Normalized();
			}

			// 追っかけ対象の座標を覚えておく
			chasePos_ = playerPos;
		}
		else
		{
			if (prevChasing_)
			{
				// パスの再計算
				pathData_ = enmMng_.GetPathFindPtr().lock()->
					PathGetR(transform_.position, chasePos_);
			}

			if (pathData_.size() > 1)
			{
				// 現在の移動先ノード
				unsigned int tgtNode = pathData_.back();

				// 移動方向
				moveDir = (pf->NodeToPos(tgtNode) - transform_.position).XZ().Normalized();

				// 現在のノードに十分近づいたら次のノードへ
				if ((pf->NodeToPos(pathData_.back()) - transform_.position).XZ().LengthSquare() <= 10.0f * 10.0f)
				{
					pathData_.pop_back();
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
	if (velocity_.XZ().LengthSquare() > 0.0f)
	{
		animation_->Stop(ResourceManager::SRC::ANIME_EN_IDLE, 0);
		animation_->Play(ResourceManager::SRC::ANIME_EN_MOVE, 0);
	}
	else
	{
		animation_->Stop(ResourceManager::SRC::ANIME_EN_MOVE, 0);
		animation_->Play(ResourceManager::SRC::ANIME_EN_IDLE, 0);
	}
}

void EnemyBase::CollisionMove()
{
	hitWall_ = true;
}

void EnemyBase::TurnVisual()
{
	if (velocity_.XZ().LengthSquare() == 0.0f) return;

	auto player = enmMng_.GetPlayerPtr();
	Vector3 moveDir = velocity_.Normalized();
	moveDir.y = 0.0f;

	Quaternion qRot = LookRotation(moveDir);

	transform_.rotation.y = qRot.ToEuler().y;

	transform_.Update();
}

void EnemyBase::AttackPlayer()
{
	if ((enmMng_.GetPlayerPtr()->GetTransform().position - transform_.position).LengthSquare() <=
		ATTACK_RANGE * ATTACK_RANGE &&
		!animation_->IsPlaying(ResourceManager::SRC::ANIME_EN_PUNCH, 0))
	{
		animation_->Play(ResourceManager::SRC::ANIME_EN_PUNCH, 0, false);

		isCanHit_ = true;
	}

	if (isCanHit_)
	{
		Position3 armPos = VECToVec3(MV1GetFramePosition(transform_.modelId,
			MV1SearchFrame(transform_.modelId, "mixamorig:RightHand")));

		auto cols = enmMng_.GetPlayerPtr()->GetTypedColliders(Collider3D::TYPE::PLAYER);

		for (auto& col : cols)
		{
			auto hit = MV1CollCheck_Sphere(
				col->GetColliderData().modelId, -1,
				Vec3ToVEC(armPos), ATTACK_HIT_RADIUS);

			if (hit.HitNum)
			{
				enmMng_.GetPlayerPtr()->CalcHP(-damage_);

				isCanHit_ = false;

				break;
			}
		}
	}
}
