#pragma once
#include <string>
#include "../../../Common/Geometry.h"
#include "../../../Manager/InputManager.h"
#include "../ActorBase3D.h"

class ItemBase : public ActorBase3D
{
public:
	enum class SHOT_TYPE
	{
		FULL_AUTO = -1,
		SEMI_AUTO = 1,
	};

	struct ItemData
	{
		std::string name = "_NAME_";
		std::string description = "_DESC_";

		int model = 0;
		Vector3 scale = { 1.0f, 1.0f, 1.0f };
		Vector3 offset = { 0.0f, 0.0f, 0.0f };
		Vector3 muzzle = { 0.0f, 0.0f, 0.0f };

		int price = 200;
		float movement = 1.0f;

		std::vector<int> fireMode = { 1 };
		int chambering = 0;
		float damage = 30;
		int penetration = 0;
		int shots = 1;
		int clipSize = 40;
		int reserve = 200;
		float dispersionRad = 0.01f;
		std::vector<float> fireRate = { 60.0f / 800.0f };
		float velocity = 4000.0f;
		float reloadTime = 2.0f;
		Vector3 recoilKick = { 0.0f, 0.0f, 0.0f };
		Vector3 recoilShake = { 0.0f, 0.0f, 0.0f };
	};

	ItemBase(ItemData data) : baseData_(data) {}

	void Update() override;
	void LateUpdate() override;
	void Draw() override;

	virtual bool Main(int pad_num, InputManager::TAGS tag);
	virtual void Sub();
	virtual void Alt();
	virtual void Reload();
	virtual void CycleFiremode();

	void Refill();

	const std::pair<const int&, const int&> GetAmmo() const { return { ammoClip_, ammoReserve_ }; }
	const std::string& GetFiremodeLabel() const { return firemodeLabel_; }
	const ItemData& GetData() const { return baseData_; }
	bool IsFiring() const { return fireCount_ < fireCountTarget_; }
	void SetParentTransform(Transform3D& parent) { parent_ = &parent; }
	float GetReloadProg() const;

protected:
	static constexpr const char* FIREMODE_LABEL_AUTO = "AUTO";
	static constexpr const char* FIREMODE_LABEL_AUTO_ALT = "ADVERSE";
	static constexpr const char* FIREMODE_LABEL_BURST = " BR";
	static constexpr const char* FIREMODE_LABEL_SEMI = "SEMI";
	static constexpr Position3 HAND_OFFSET = { 3.5f, 22.5f, -12.0f };

	Transform3D* parent_;

	Vector3 muzzlePos_;

	ItemData baseData_;

	int ammoClip_;
	int ammoReserve_;
	float shotTimer_ = 0.0f;
	float reloadProg_ = 0.0f;

	bool muzzleLock_ = false;
	unsigned int firemode_ = 0u;
	std::string firemodeLabel_ = "";
	int fireCountTarget_ = -1;
	int fireCount_ = -1;

	void InitParam() override;
	void InitModel() override;
	void InitAnim() override {}

	void Collision() override;

	void TurnVisual() override;

	void _SetFiremodeLabel();

	float _FireRate();

	Vector3 GetEuler(const Matrix4x4& m);

};