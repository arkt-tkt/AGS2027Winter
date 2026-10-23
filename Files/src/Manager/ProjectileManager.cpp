#include "ProjectileManager.h"

ProjectileManager* ProjectileManager::instance_ = nullptr;

ProjectileManager::ProjectileManager()
{
	pool_.resize(POOL_SIZE);
	for (auto& cell : pool_) cell = std::make_unique<Projectile>();

	active_.reserve(POOL_SIZE);
}

void ProjectileManager::Update()
{
	for (auto it = active_.begin(); it != active_.end(); )
	{
		if (!(*it)->IsAlive())
		{
			it = active_.erase(it);
			continue;
		}

		(*it)->Update();

		if (!(*it)->IsAlive())
		{
			it = active_.erase(it);
			continue;
		}

		++it;
	}
}

void ProjectileManager::Draw()
{
	for (auto& cell : active_)
	{
		cell->Draw();
	}
}

void ProjectileManager::Create(Projectile::Data dat)
{
	for (auto& cell : pool_) if (!cell->IsAlive())
	{
		cell->SetData(dat);
		cell->Init();
		active_.push_back(cell.get());
		return;
	}
}
