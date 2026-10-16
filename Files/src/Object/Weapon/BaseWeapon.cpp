#include "../../Common/MathUtil.h"
#include "../../Manager/SceneManager.h"
#include "BaseWeapon.h"

BaseWeapon::BaseWeapon(WeaponData dat)
{
	editData_ = baseData_ = dat;
}

void BaseWeapon::Update()
{
}

void BaseWeapon::Draw()
{
}

void BaseWeapon::Main()
{
	if (editData_.reloadTime) return;

	editData_.clipSize = MathUtil::Clamp(editData_.clipSize - 1, 0, baseData_.clipSize);
}

void BaseWeapon::Sub()
{
	if (editData_.reloadTime) return;

	// ADSèàóù
}

void BaseWeapon::Alt()
{
}

void BaseWeapon::Reload()
{
	if (!editData_.reloadTime)
	{
		editData_.reloadTime = baseData_.reloadTime;
	}
	else
	{
		auto dt = SceneManager::GetInstance().GetDeltaTime();

		editData_.reloadTime -= dt;

		if (editData_.reloadTime <= 0.0f)
		{
			editData_.reloadTime = 0.0f;
			editData_.clipSize = baseData_.clipSize;
		}
	}
}

float BaseWeapon::GetReloadProg() const
{
	if (editData_.reloadTime == -1.0f)
	{
		return -1.0f;
	}
	else
	{
		return (baseData_.reloadTime - editData_.reloadTime) / baseData_.reloadTime;
	}
}
