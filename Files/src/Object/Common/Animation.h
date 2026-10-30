#pragma once
#include <map>
#include <string>
#include <vector>
#include "../../Common/Geometry.h"
#include "../../Manager/ResourceManager.h"

class Animation
{
public:
	enum class BLEND_TYPE
	{
		TRANSITION,

		UPPER_BODY,
		ARMS,
		LOWER_BODY,
	};

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
	void Blend(ResourceManager::SRC src, int type, bool isLoop = true);
	bool IsPlaying() const;
	bool IsPlaying(ResourceManager::SRC src, int type) const;

	Vector3 GetAnimLocalPos(int frame) const;
	void SetPlayingAnimSpeed(float speed = 1.0f) { speed_ = speed; }

private:
	static constexpr int PLAYDATA_RESERVE = 2;
	static constexpr float TRANSITION_TIME = 0.1f;

	int modelId_;

	std::map<ResourceManager::SRC, std::map<int, AnimData>> anims_;

	std::pair<ResourceManager::SRC, int> playType_ = { ResourceManager::SRC::ANIME_PL_IDLE, -1 };
	AnimData playAnim_;

	using AnimPair = std::pair<ResourceManager::SRC, int>;

	struct PlayData
	{
		AnimPair playType = { ResourceManager::SRC::ANIME_PL_IDLE, -1 };
		AnimData playAnim;
		bool isLoop;
		float blendRate;
	};
	std::vector<PlayData> playDatas_;

	bool isLoop_;
	float speed_ = 1.0f;

	int AttachAnim(int anim_model);
	int DetachAnim(int anim_model);

};