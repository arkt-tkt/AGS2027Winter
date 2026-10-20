#pragma once
#include <map>
#include <string>
#include <vector>
#include "../../Manager/ResourceManager.h"

class Animation
{
public:
	struct AnimData
	{
		int model = -1;
		int attachIdx = -1;
		int animType = 0;
		float speed = 0.0f;
		float totalTime = 0.0f;
		float step = 0.0f;
	};

	Animation(int modelId);
	~Animation();

	void AddFromMine(int type, float speed);
	void AddFromOther(ResourceManager::SRC src, int type, float speed);

	void Update();

	void Play(ResourceManager::SRC src, int type, bool isLoop = true);
	bool IsPlaying() const;
	bool IsPlaying(ResourceManager::SRC src, int type) const;

	void SetPlayingAnimSpeed(float speed = 1.0f) { speed_ = speed; }

private:
	int modelId_;

	std::map<ResourceManager::SRC, std::map<int, AnimData>> anims_;

	std::pair<ResourceManager::SRC, int> playType_ = { ResourceManager::SRC::ANIME_PL_IDLE, -1 };
	AnimData playAnim_;

	bool isLoop_;
	float speed_;

	int AttachAnim(int anim_model);

};