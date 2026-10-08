#pragma once
#include <fstream>
#include <map>
#include <Windows.h>
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
	};

	// リソース登録
	void Register(ResourceManager::SRC src);
	// リソース一斉登録
	void RegisterAll();

	// 音源を再生
	void Play(ResourceManager::SRC src, int play_from_top = TRUE);
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

	float volumeBGM_;
	float volumeSE_;

	int VolumeMultiplication(float f);

	void ApplyBGMVolume();
	void ApplySEVolume();

	// 値をレベルに変換
	int ValueToLevel(float value) const;
	// レベルを値に変換
	float LevelToValue(int level) const;

};
