#pragma once
#include <memory>
#include <vector>
#include "../../Common/Geometry.h"

class CapturePoint;
class Collider3D;
class GameScene;
class Platform;
class Player;

class Stage
{
public:
	Stage(GameScene* ptr);

	void Init();
	void Update();
	void Draw();
	void DrawUI();

	std::vector<std::shared_ptr<Collider3D>> GetColliders() const;
	int GetCaptureCount();

	void SetPlayerPtr(std::weak_ptr<Player> player) { player_ = player; }
	Player* GetPlayerPtr() const { return player_.lock().get(); }

private:
	static constexpr int CAPTURE_POINT_NUM = 5;

	static constexpr Vector3 CAPTURE_POINT_POS[CAPTURE_POINT_NUM] = 
	{
		{ -700, 0, 1650 },
		{ -2100, 0, 350 },
		{ -700, 0, -2400 },
		{ 1050, 0, -600 },
		{ 150, 0, 900 }
	};

	GameScene* gameScene_;
	std::weak_ptr<Player> player_;

	std::shared_ptr<Platform> platform_;
	std::vector<std::shared_ptr<CapturePoint>> capturePoints_;

};