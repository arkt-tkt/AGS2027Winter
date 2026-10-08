#include "../Manager/ResourceManager.h"
#include "../Manager/SceneManager.h"
#include "Common/Collider2D.h"
#include "ActorBase2D.h"

ActorBase2D::ActorBase2D()
	:
	resMng_(ResourceManager::GetInstance()),
	scnMng_(SceneManager::GetInstance())
{
}

ActorBase2D::~ActorBase2D()
{
	awayColliders_.clear();
}

const Object2D& ActorBase2D::GetObject2D() const
{
	return object2D_;
}
