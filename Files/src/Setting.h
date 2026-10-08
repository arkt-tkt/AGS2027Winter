#pragma once
#include <vector>

class Setting
{
public:
	static void CreateInstance(int pad_max) { if (instance_ == nullptr) instance_ = new Setting(); }
	static Setting& GetInstance() { return *instance_; }
	static void DeleteInstance() { if (instance_ != nullptr) delete instance_; instance_ = nullptr; }

private:
	static Setting* instance_;

	Setting() {}
	~Setting() {}

	Setting(const Setting&) = delete;
	Setting& operator=(const Setting&) = delete;
	Setting(Setting&&) = delete;
	Setting& operator=(Setting&&) = delete;

public:
	struct Settings {

	} settings_;

	// バイナリデータ読み込み
	void LoadBinary(const char* path);

	Settings& GetSettingData();

private:

};

