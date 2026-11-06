#pragma once
#include <fstream>
#include <map>
#include <Windows.h>
#include "../Common/Geometry.h"
#include "ResourceManager.h"

class AudioManager {
private:
	static AudioManager* instance_;

	AudioManager();
	~AudioManager();

	AudioManager(const AudioManager&) = delete;
	AudioManager& operator=(const AudioManager&) = delete;
	AudioManager(AudioManager&&) = delete;
	AudioManager& operator=(AudioManager&&) = delete;

public:
	static void CreateInstance() { if (instance_ == nullptr) instance_ = new AudioManager; }
	static AudioManager& GetInstance() { return *instance_; }
	static void DeleteInstance() { if (instance_ != nullptr) delete instance_; instance_ = nullptr; }

	struct SOUND_DATA
	{
		int handle;
		Resource::TYPE type;
		float volMult;
		int playType;

		// 3D音源追跡用
		bool use3d;
		Position3 pos;
		float range;
	};

	// リソース登録
	void Register(ResourceManager::SRC src);
	// リソース一斉登録
	void RegisterAll();

	void Set3DListenerPos(const Vector3& pos, const Vector3& front, const Vector3& up);

	void Update();

	// 音源を再生
	void Play(ResourceManager::SRC src, int play_from_top = TRUE);
	// 3D空間上で音源を再生
	void Play3D(ResourceManager::SRC src, const Vector3& pos, float range = 1000.0f, int play_from_top = TRUE);
	// 再生中の音源を停止
	void Stop(ResourceManager::SRC src);

	float GetBGMVolume() const;
	void SetBGMVolume(float value);
	float GetSEVolume() const;
	void SetSEVolume(float value);

	int GetBGMVolumeLevel() const;
	void SetBGMVolumeLevel(int level);
	int GetSEVolumeLevel() const;
	void SetSEVolumeLevel(int level);

private:
	static constexpr float VOLUME_MULT = 255.0f;
	static constexpr int MAX_VOLUME_LEVEL = 8;
	static constexpr float VOLUME_LEVEL_MULT = 1.0f / MAX_VOLUME_LEVEL;

	std::map<ResourceManager::SRC, SOUND_DATA> soundMap_;

	Vector3 listenerPos_;
	Vector3 listenerFront_;
	Vector3 listenerUp_;

	float volumeBGM_;
	float volumeSE_;

	// 内部再生
	void _Play(std::map<ResourceManager::SRC, SOUND_DATA>::iterator it, int play_from_top = TRUE);

	int SoundVolumeMult(float f);

	void ApplyBGMVolume();
	void ApplySEVolume();

	// 値をレベルに変換
	int ValueToLevel(float value) const;
	// レベルを値に変換
	float LevelToValue(int level) const;

};
