#pragma once
#include <string>
#include <vector>
#include "../External/json/json.hpp"

class Resource
{

public:
	
	// リソースタイプ
	enum class TYPE
	{
		// 無し
		NONE,
		// DXライブラリ用フォント
		DXFONT,
		// Effekseer製エフェクト
		EFFEKSEER,
		// フォント
		FONT,
		// 画像
		IMAGE,
		// 複数画像
		IMAGES,
		// 3Dモデル
		MODEL,
		// 音楽
		MUSIC,
		// ピクセルシェーダ
		PIXEL_SHADER,
		// 効果音
		SE,
		// JSONファイル
		JSON
	};

	Resource() {};

	// コンストラクタ(汎用)
	Resource(TYPE type, const std::string& path);
	// コンストラクタ(DXFONT用)
	Resource(TYPE type, const std::string& path, int fontEdge);
	// コンストラクタ(FONT用)
	Resource(TYPE type, const std::string& path, const std::string& fontName, int fontSize, int fontThick, int fontEdge);
	// コンストラクタ(IMAGES用)
	Resource(TYPE type, const std::string& path, int numX, int numY, int sizeX, int sizeY);
	// コンストラクタ(MUSIC/SE用)
	Resource(TYPE type, const std::string& path, float volumeMult, bool playLoop);
	// コンストラクタ(PIXEL_SHADER用)
	Resource(TYPE type, const std::string& path, unsigned int constBufferSize);

	// デストラクタ
	~Resource();

	// 読み込み
	void Load();

	// 解放
	void Release();

	// 複数画像ハンドルを別配列にコピー
	void CopyHandle(std::vector<int>& imgs) const;

	// 汎用
	TYPE type_ = TYPE::NONE; // リソースタイプ
	std::string path_ = ""; // リソースの読み込み先
	int handleId_ = -1; // リソースのハンドルID

	// AUDIO用
	float volumeMult_ = 0; // 音量
	int playType_ = 0; // 再生タイプ

	// DXFONT/FONT用
	int fontSize_ = 16; // フォントの大きさ
	int fontThick_ = -1; // フォントの太さ
	int fontEdge_ = 0; // フォントのエッジサイズ
	std::string fontName_ = ""; // フォントの正式名称

	// IMAGES用
	std::vector<int> handleIds_ = {};
	int numX_ = 0;
	int numY_ = 0;
	int sizeX_ = 0;
	int sizeY_ = 0;

	// MODEL用
	std::vector<int> duplicateModelIds_ = {}; // モデル複製用

	// PIXEL_SHADER用
	int constBufferId_ = -1;
	unsigned int constBufferSize_ = 0u;

	// JSON用
	nlohmann::json json_ = {};

};
