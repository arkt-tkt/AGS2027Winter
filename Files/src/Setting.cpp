#include <DxLib.h>
#include "Setting.h"

void Setting::LoadBinary(const char* path)
{
	auto handle = FileRead_open(path);



	FileRead_close(handle);
}

Setting::Settings& Setting::GetSettingData()
{
	return settings_;
}
