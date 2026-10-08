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

	void Play(ResourceManager::SRC src, int type, bool isLoop = true);

	void Update();

	void SetPlayingAnimSpeed(float speed = 1.0f);

private:
	int modelId_;

	std::map<ResourceManager::SRC, std::map<int, AnimData>> anims_;

	std::pair<ResourceManager::SRC, int> playType_;
	AnimData playAnim_;

	bool isLoop_;
	float speed_;

	int AttachAnim(int anim_model);

};