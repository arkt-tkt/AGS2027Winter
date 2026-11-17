#include "../Common/DxLibUtil.h"
#include "../Common/MathUtil.h"
#include "SceneManager.h"
#include "ProjectileManager.h"

ProjectileManager* ProjectileManager::instance_ = nullptr;

ProjectileManager::ProjectileManager()
{
	pool_.resize(POOL_SIZE);
	for (auto& cell : pool_) cell = std::make_unique<Projectile>(*this);

	active_.reserve(POOL_SIZE);

	popUps_.reserve(60ull);
}

void ProjectileManager::Update()
{
	auto dt = SceneManager::GetInstance().GetDeltaTime();

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

	for (auto it = popUps_.begin(); it != popUps_.end(); )
	{
		(*it).timer -= dt;
		if ((*it).timer <= 0.0f) it = popUps_.erase(it);
		else ++it;
	}
}

void ProjectileManager::Draw()
{
	for (auto& cell : active_)
	{
		cell->Draw();
	}
}

void ProjectileManager::DrawUI()
{
	for (auto& cell : popUps_)
	{
		// オーバーレイ表示
		VECTOR scPos = ConvWorldPosToScreenPos(Vec3ToVEC(cell.pos));

		if (!SceneManager::GetInstance().GetCameraPtr().IsInCameraBounds(VECToVec3(scPos))) return;

		DrawStringToCenterPos(MathUtil::Round(scPos.x), MathUtil::Round(scPos.y),
			std::to_string(MathUtil::Round(cell.damage)).c_str(), 0xffffffu);
	}
}

bool ProjectileManager::CreateProjectile(Projectile::Data dat)
{
	for (auto& cell : pool_) if (!cell->IsAlive())
	{
		cell->SetData(dat);
		cell->Init();
		active_.push_back(cell.get());
		return true;
	}

	return false;
}

void ProjectileManager::CreateDamagePopUp(const Position3& pos, float damage)
{
	PopUpData cell = { POP_UP_TIMER, damage, pos };
	popUps_.emplace_back(cell);
}
