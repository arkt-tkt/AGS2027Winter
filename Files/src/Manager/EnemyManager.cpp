#include "../Scene/SceneBase.h"
#include "../Scene/GameScene/GameScene.h"
#include "SceneManager.h"
#include "EnemyManager.h"

EnemyManager* EnemyManager::instance_ = nullptr;

EnemyManager::EnemyManager(GameScene* ptr)
	:
	gameScene_(ptr)
{
	pool_.resize(POOL_SIZE);
	for (auto& cell : pool_) cell = std::make_unique<EnemyBase>(this);

	active_.reserve(POOL_SIZE);
}

void EnemyManager::Update()
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

	spawnTimer_ += SceneManager::GetInstance().GetDeltaTime();
	if (spawnTimer_ >= SPAWN_INTERVAL)
	{
		spawnTimer_ = 0.0f;
		Spawn();
	}
}

void EnemyManager::Draw()
{
	for (auto& cell : active_)
	{
		cell->Draw();
	}
}

void EnemyManager::Spawn()
{
	// ‚Æ‚è‚ ‚¦‚¸1Ží—Þ‚ð’èŠú“I‚É¶¬‚·‚é
	for (auto& cell : pool_)
	{
		if (cell == nullptr || !cell->IsAlive())
		{
			cell = std::make_unique<EnemyBase>(this);
			cell->Init();
			active_.push_back(cell.get());
			return;
		}
	}
}
