#include "../../../../Common/DxLibUtil.h"
#include "../../../../Manager/EnemyManager.h"
#include "../../../../Manager/SceneManager.h"
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
#endif
	
	CharacterBase3D::Draw();
}

void EnemyBase::DrawUI()
{
}

void EnemyBase::InitParam()
{
	isAlive_ = true;
	maxHp_ = hp_ = 150;
	maxSpeed_ = 132.0f;
	fullSpeedParallel = true;
}

void EnemyBase::InitModel()
{
	transform_.modelId = resMng_.LoadModelDuplicate(ResourceManager::SRC::MODEL_ENEMY1);
	transform_.scale = SCALE;
	transform_.position = Vector3(0, 800, 0);
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
	animation_ = std::make_unique<Animation>(transform_.modelId);

	animation_->AddFromOther(ResourceManager::SRC::ANIME_PL_IDLE, 0, 30.0f);

	animation_->Play(ResourceManager::SRC::ANIME_PL_IDLE, 0);
}

void EnemyBase::InitOther()
{
}

void EnemyBase::Move()
{
	auto player = enmMng_.GetPlayerPtr();
	Vector2 moveDir = (player->GetTransform().position.XZ() - transform_.position.XZ()).Normalized();

	moveDir *= maxSpeed_ * scnMng_.GetDeltaTime();

	velocity_.x = moveDir.x;
	velocity_.z = moveDir.y;

	if (jumpNextFrame_)
	{
		jumpNextFrame_ = false;
		Jump();
	}
}

void EnemyBase::AnimationSwitch()
{
}

void EnemyBase::CollisionMove()
{
	jumpNextFrame_ = true;
}

void EnemyBase::TurnVisual()
{
	auto player = enmMng_.GetPlayerPtr();
	Vector3 moveDir = (player->GetTransform().position - transform_.position).Normalized();
	moveDir.y = 0.0f;

	Quaternion qRot = LookRotation(moveDir);

	transform_.rotation.y = qRot.ToEuler().y;

	transform_.Update();
}
