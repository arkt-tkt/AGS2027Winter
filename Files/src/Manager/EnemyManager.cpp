#include "../Scene/GameScene/GameScene.h"
#include "../Scene/SceneBase.h"
#include "../Utility/RNG.h"
#include "../Application.h"
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

	_LoadValue();
}

EnemyManager::~EnemyManager()
{
	data_.clear();

	active_.clear();

	for (auto& cell : pool_)
	{
		cell->Release();
	}
	pool_.clear();
}

void EnemyManager::Update()
{
	for (auto it = active_.begin(); it != active_.end(); )
	{
		(*it)->Update();

		if (!(*it)->IsAlive())
		{
			(*it)->Release();
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

void EnemyManager::LateUpdate()
{
	for (auto it = active_.begin(); it != active_.end(); ++it)
	{
		(*it)->LateUpdate();
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
	auto ret = Application::GetInstance().GetRNGPointer().GetRandUInt(
		(uint32_t)data_.size() - 1);

	// ‚Æ‚è‚ ‚¦‚¸1Ží—Þ‚ð’èŠú“I‚É¶¬‚·‚é
	for (auto& cell : pool_)
	{
		if (cell == nullptr || !cell->IsAlive())
		{
			cell = std::make_unique<EnemyBase>(this);
			cell->baseStats_ = data_[ret];
			cell->Init();
			for (auto& col : awayColliders_) cell->AddAwayCollider(col);
			active_.push_back(cell.get());
			return;
		}
	}
}

void EnemyManager::TriggerSoundEvent(const Position3& pos, SOUND_EVENT event)
{
	float range = static_cast<float>(event);

	for (auto& cell : active_)
	{
		if ((cell->GetTransform().position - pos).LengthSquare() <= range * range)
		{
			cell->GetSoundPos(pos);
		}
	}
}

void EnemyManager::AddAwayColliders(std::vector<std::shared_ptr<Collider3D>> cols)
{
	for (auto& col : cols)
	{
		awayColliders_.push_back(col);
	}
}

void EnemyManager::_LoadValue()
{
	auto& rawData = ResourceManager::GetInstance().Reload(ResourceManager::SRC::JSON_ENEMIES).json_;

	for (auto& [key, data] : rawData.items())
	{
		CharacterBase3D::Stats stats;

		std::string keyName = "Model";
		if (data.contains(keyName))
			stats.model = data[keyName].get<int>();

		keyName = "Scale";
		if (data.contains(keyName))
		{
			if (data[keyName].is_array() && data[keyName].size() == 3)
			{
				stats.scale = Vector3(
					data[keyName][0].get<float>(),
					data[keyName][1].get<float>(),
					data[keyName][2].get<float>());
			}
			else
			{
				stats.scale = data[keyName].get<float>();
			}
		}

		keyName = "Bounty";
		if (data.contains(keyName))
			stats.bounty = data[keyName].get<int>();

		keyName = "HP";
		if (data.contains(keyName))
			stats.hp = data[keyName].get<float>();

		keyName = "Regen";
		if (data.contains(keyName))
			stats.regen = data[keyName].get<float>();

		keyName = "Armor";
		if (data.contains(keyName))
			stats.armor = data[keyName].get<float>();

		keyName = "Damage";
		if (data.contains(keyName))
			stats.damage = data[keyName].get<float>();

		keyName = "AttackSpeed";
		if (data.contains(keyName))
			stats.attackSpeed = data[keyName].get<float>();

		keyName = "Movement";
		if (data.contains(keyName))
			stats.movement = data[keyName].get<float>();

		data_.emplace_back(stats);
	}
}
