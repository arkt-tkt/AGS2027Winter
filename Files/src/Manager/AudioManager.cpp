#include <algorithm>
#include <cmath>
#include <DxLib.h>
#include "../Common/MathUtil.h"
#include "AudioManager.h"

AudioManager* AudioManager::instance_ = nullptr;

// ★修正：起動時に自動で設定ファイルを読み込むようにする
AudioManager::AudioManager()
{
	volumeBGM_ = 1.0f;
	volumeSE_ = 1.0f;
}

AudioManager::~AudioManager()
{
	// サウンドハンドルのメモリ管理は ResourceManager に完全移行したため、
	// ここでの DeleteSoundMem は行わずリストの初期化のみにします。
	soundMap_.clear();
}

void AudioManager::Register(ResourceManager::SRC src)
{
	auto& map = ResourceManager::GetInstance().GetLoadedMap();
	auto it = map.find(src);

	// ロードされていない場合は処理しない
	if (it == map.end()) return;

	// リソースが音楽以外の場合は処理しない
	if ((*it).second.type_ != Resource::TYPE::MUSIC &&
		(*it).second.type_ != Resource::TYPE::SE) return;

	// サウンドデータ作成
	SOUND_DATA data = {};
	data.handle = (*it).second.handleId_;
	data.type = (*it).second.type_;
	data.volMult = (*it).second.volumeMult_;
	data.playType = (*it).second.playType_;
	
	// 登録する前に、現在の設定音量を反映させる
	switch ((*it).second.type_)
	{
	case Resource::TYPE::MUSIC:
		ChangeVolumeSoundMem(VolumeMultiplication(volumeBGM_ * data.volMult), data.handle);
		break;
	case Resource::TYPE::SE:
		ChangeVolumeSoundMem(VolumeMultiplication(volumeSE_ * data.volMult), data.handle);
		break;
	}

	// 登録
	soundMap_[(*it).first] = data;
}

void AudioManager::RegisterAll()
{
	auto& map = ResourceManager::GetInstance().GetLoadedMap();

	for (auto& pair : map)
	{
		// リソースが音楽以外の場合は処理しない
		if (pair.second.type_ == Resource::TYPE::MUSIC ||
			pair.second.type_ == Resource::TYPE::SE)
		{
			// サウンドデータ作成
			SOUND_DATA data = {};
			data.handle = pair.second.handleId_;
			data.type = pair.second.type_;
			data.volMult = pair.second.volumeMult_;
			data.playType = pair.second.playType_;

			soundMap_[pair.first] = data;
		}
	}

	// 一括登録が終わった直後に、現在の設定音量を適用する
	ApplyBGMVolume();
	ApplySEVolume();
}

void AudioManager::Play(ResourceManager::SRC src, int play_from_top)
{
	auto it = soundMap_.find(src);
	if (it == soundMap_.end()) return;

	switch ((*it).second.type)
	{
	case Resource::TYPE::MUSIC:
		if (!CheckSoundMem((*it).second.handle))
		{
			PlaySoundMem((*it).second.handle, (*it).second.playType, play_from_top);
		}
		break;
	case Resource::TYPE::SE:
		PlaySoundMem((*it).second.handle, (*it).second.playType);
		break;
	}
}

void AudioManager::Stop(ResourceManager::SRC src)
{
	auto it = soundMap_.find(src);
	if (it == soundMap_.end()) return;

	if (CheckSoundMem((*it).second.handle))
	{
		StopSoundMem((*it).second.handle);
	}
}

float AudioManager::GetBGMVolume() const
{
	return volumeBGM_;
}

void AudioManager::SetBGMVolume(float value)
{
	volumeBGM_ = value;
	ApplyBGMVolume();
}

float AudioManager::GetSEVolume() const
{
	return volumeSE_;
}

void AudioManager::SetSEVolume(float value)
{
	volumeSE_ = value;
	ApplySEVolume();
}

int AudioManager::GetBGMVolumeLevel() const
{
	return ValueToLevel(volumeBGM_);
}

void AudioManager::SetBGMVolumeLevel(int level)
{
	volumeBGM_ = LevelToValue(level);
	ApplyBGMVolume();
}

int AudioManager::GetSEVolumeLevel() const
{
	return ValueToLevel(volumeSE_);
}

void AudioManager::SetSEVolumeLevel(int level)
{
	volumeSE_ = LevelToValue(level);
	ApplySEVolume();
}

int AudioManager::VolumeMultiplication(float f)
{
	return int(VOLUME_MULT * f);
}

void AudioManager::ApplyBGMVolume()
{
	for (auto& pair : soundMap_)
	{
		if (pair.second.type == Resource::TYPE::MUSIC)
		{
			ChangeVolumeSoundMem(VolumeMultiplication(volumeBGM_ * pair.second.volMult), pair.second.handle);
		}
	}
}

void AudioManager::ApplySEVolume()
{
	for (auto& pair : soundMap_)
	{
		if (pair.second.type == Resource::TYPE::SE)
		{
			ChangeVolumeSoundMem(VolumeMultiplication(volumeSE_ * pair.second.volMult), pair.second.handle);
		}
	}
}

int AudioManager::ValueToLevel(float value) const
{
	return MathUtil::Round(value / VOLUME_LEVEL_MULT);
}

float AudioManager::LevelToValue(int level) const
{
	return level * VOLUME_LEVEL_MULT;
}
