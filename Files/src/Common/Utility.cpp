#include <Windows.h>
#include "Utility.h"

std::wstring Utility::GetWideStringFromString(const std::string& str)
{
    // MultiByteToWideChar関数はマルチバイト文字列をワイド文字列に変換しますが、2回呼び出す必要がある関数です。
    // 
    // 1回目の呼び出しで、ワイド文字列のサイズを取得します。この時はサイズが分からないため、受け取るための引数にnullptrを指定して、文字列長を得ます。
    // 文字列長が分かったら、ワイド文字列の領域を確保しておきます。
    // 2回目の呼び出しで、実際に文字列を代入します。

    std::wstring wstr;

    int count = MultiByteToWideChar(
        CP_ACP,
        MB_COMPOSITE | MB_ERR_INVALID_CHARS,
        str.c_str(),
        str.length(),
        nullptr, // ここにnullptrを指定すると、文字列長の取得が目的であることを示す
        0
    );

    wstr.resize(count); // 必要な文字列長を確保

    MultiByteToWideChar(
        CP_ACP,
        MB_COMPOSITE | MB_ERR_INVALID_CHARS,
        str.c_str(),
        str.length(),
        wstr.data(), // 代入したい文字列のアドレス
        wstr.size() // 代入したい文字列の領域長
    );

    return wstr;
}

std::string Utility::GetStringFromWideString(const std::wstring& wstr)
{
    // 使い方はGetWideStringFromString関数のコメントを確認して下さい。

    std::string str;

    int count = WideCharToMultiByte(
        CP_ACP,
        0, // MultiByteToWideCharとは異なり、ここには何も入れない
        wstr.c_str(),
        wstr.length(),
        nullptr,
        0,
        nullptr,
        nullptr
    );

    str.resize(count);

    WideCharToMultiByte(
        CP_ACP,
        0,
        wstr.c_str(),
        wstr.length(),
        str.data(),
        str.size(),
        nullptr,
        nullptr
    );

    return str;
}
