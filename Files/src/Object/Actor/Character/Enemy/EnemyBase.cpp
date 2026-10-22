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

	MV1DrawModel(transform_.modelId);
}

void EnemyBase::DrawUI()
{
}

void EnemyBase::InitParam()
{
	isAlive_ = true;
	maxHp_ = hp_ = 15;
	maxSpeed_ = 132.0f;
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
			transform_.position + Vector3(0, 24, 0),
			transform_.position + Vector3(0, 60, 0),
			8.0f));

	colliders_.emplace_back(
		std::make_shared<Collider3D>(
			Collider3D::TYPE::ENEMY,
			transform_.modelId));

	lineCollider_ = {
		Vector3(0, -4, 0),
		Vector3(0, 68, 0),
	};
}

void EnemyBase::InitAnim()
{
}

void EnemyBase::InitOther()
{
}

void EnemyBase::Move()
{
	auto player = enmMng_.GetPlayerPtr();

	Vector3 moveDir = (player->GetTransform().position - transform_.position).Normalized();

	moveDir *= maxSpeed_ * scnMng_.GetDeltaTime();

	velocity_.x = moveDir.x;
	velocity_.z = moveDir.z;
}

void EnemyBase::AnimationSwitch()
{
}

void EnemyBase::CollisionMove()
{
	Jump();
}

void EnemyBase::TurnVisual()
{
	Quaternion qRot = LookRotation(velocity_.Normalized());
	transform_.rotation = qRot.ToEuler();

	transform_.Update();
}
