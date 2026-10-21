#pragma once
#include <memory>
#include <vector>

class Projectile;

class ProjectileManager
{
public:
	// シングルトン
	static void CreateInstance() { if (instance_ == nullptr) instance_ = new ProjectileManager(); }
	static ProjectileManager& GetInstance() { return *instance_; }
	static void DeleteInstance() { if (instance_ != nullptr) delete instance_; instance_ = nullptr; }

private:
	// インスタンス
	static ProjectileManager* instance_;

	// シングルトン
	ProjectileManager(const ProjectileManager&) = delete;
	ProjectileManager& operator=(const ProjectileManager&) = delete;
	ProjectileManager(ProjectileManager&&) = delete;
	ProjectileManager& operator=(ProjectileManager&&) = delete;

	// コンストラクタ
	ProjectileManager();
	// デストラクタ
	virtual ~ProjectileManager() {}

public:
	void Update();

	void CreateProjectile(Projectile::Data);
	std::vector<Projectile*> GetActiveProjectiles() { return active_; }

private:
	static constexpr size_t POOL_SIZE = 120;

	std::vector<std::unique_ptr<Projectile>> pool_;
	std::vector<Projectile*> active_;
};