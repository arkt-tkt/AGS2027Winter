#pragma once
#include <string>

// 個人的汎用関数用namespace
namespace Utility
{
	// 文字列(Unicode)をワイド文字列に変換
	std::wstring GetWideStringFromString(const std::string& str);
	// ワイド文字列を文字列(Unicode)に変換
	std::string GetStringFromWideString(const std::wstring& wstr);
}