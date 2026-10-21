#include "../../../Common/DxLibUtil.h"
#include "Projectile.h"

void Projectile::Draw()
{
	DrawSphere3D(Vec3ToVEC(transform_.position), dat_.radian, 8, 0xff0000u, 0xffffffu, TRUE);
}

void Projectile::InitOther()
{
	velocity_ = dat_.velocity;
}
