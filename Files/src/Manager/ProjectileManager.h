#pragma once
#include <memory>
#include <vector>
#include "../Object/Actor/Projectile/Projectile.h"

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
	void Draw();

	// プール内に投射物を生成する
	// @return 生成に成功したか(falseの場合、空き領域が無い)
	bool Create(Projectile::Data);
	const std::vector<Projectile*>& GetActives() { return active_; }

private:
	static constexpr size_t POOL_SIZE = 150;

	std::vector<std::unique_ptr<Projectile>> pool_;
	std::vector<Projectile*> active_;
};