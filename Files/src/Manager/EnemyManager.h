#pragma once
#include <memory>
#include <vector>
#include "../Object/Actor/Character/Enemy/EnemyBase.h"

class GameScene;
class Player;

class EnemyManager
{
public:
	// シングルトン
	static void CreateInstance(GameScene* ptr) { if (instance_ == nullptr) instance_ = new EnemyManager(ptr); }
	static EnemyManager& GetInstance() { return *instance_; }
	static void DeleteInstance() { if (instance_ != nullptr) delete instance_; instance_ = nullptr; }

private:
	// インスタンス
	static EnemyManager* instance_;

	// シングルトン
	EnemyManager(const EnemyManager&) = delete;
	EnemyManager& operator=(const EnemyManager&) = delete;
	EnemyManager(EnemyManager&&) = delete;
	EnemyManager& operator=(EnemyManager&&) = delete;

	// コンストラクタ
	EnemyManager(GameScene* ptr);
	// デストラクタ
	virtual ~EnemyManager() {}

public:
	void Update();
	void Draw();

	void Spawn();

	void SetPlayerPtr(std::weak_ptr<Player> player) { player_ = player; }
	Player* GetPlayerPtr() const { return player_.lock().get(); }

private:
	static constexpr int POOL_SIZE = 10;
	static constexpr float SPAWN_INTERVAL = 1.6f;

	GameScene* gameScene_ = nullptr;
	std::weak_ptr<Player> player_;

	std::vector<std::unique_ptr<EnemyBase>> pool_;
	std::vector<EnemyBase*> active_;

	float spawnTimer_ = 0.0f;

};