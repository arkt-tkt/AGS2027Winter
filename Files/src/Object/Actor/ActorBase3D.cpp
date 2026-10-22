#include "../../Common/DxLibUtil.h"
#include "../../Manager/FPSManager.h"
#include "../../Manager/ResourceManager.h"
#include "../../Manager/SceneManager.h"
#include "ActorBase3D.h"

ActorBase3D::ActorBase3D()
	:
	resMng_(ResourceManager::GetInstance()),
	scnMng_(SceneManager::GetInstance())
{
}

ActorBase3D::~ActorBase3D()
{
	Release();
}

void ActorBase3D::Init()
{
	InitParam();

	InitModel();

	InitAnim();

	InitOther();

	transform_.Update();
}

void ActorBase3D::UpdateColliders()
{
	// Collider3D‚ÌÀ•Wî•ñXV
	for (auto& collider : colliders_)
	{
		if (collider->GetColliderData().shape != Collider3D::SHAPE::CAPSULE_ALT)
		{
			collider->Update(transform_.position, transform_.rotation);
		}
		else
		{
			collider->UpdateAlt(transform_.position, transform_.position2nd);
		}
	}
}

void ActorBase3D::Release()
{
	transform_.Release();

	for (auto& collider : colliders_)
		collider->Release();

	colliders_.clear();
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

std::vector<std::shared_ptr<Collider3D>> ActorBase3D::GetTypedColliders(Collider3D::TYPE type) const
{
	std::vector<std::shared_ptr<Collider3D>> ret;

	for (auto& collider : colliders_)
	{
		if (collider->GetColliderData().type == type)
			ret.push_back(collider);
	}

	return ret;
}
