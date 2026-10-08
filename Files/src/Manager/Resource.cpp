#include <DxLib.h>
#include <EffekseerForDXLib.h>
#include <fstream>
#include "../Application.h"
#include "ResourceManager.h"
#include "Resource.h"

Resource::Resource(TYPE type, const std::string& path)
{
	type_ = type;
	
	switch (type_)
	{
	case Resource::TYPE::IMAGE:
		path_ = Application::PATH_IMAGE + path;
		break;
	case Resource::TYPE::MODEL:
		path_ = Application::PATH_MODEL + path;
		break;
	case Resource::TYPE::EFFEKSEER:
		path_ = Application::PATH_EFFECT + path;
		break;
	case Resource::TYPE::JSON:
		path_ = Application::PATH_TEXT + path;
		break;
	}
}

Resource::Resource(TYPE type, const std::string& path, int fontEdge)
{
	type_ = type;
	path_ = Application::PATH_FONT + path;

	fontEdge_ = fontEdge;
}

Resource::Resource(TYPE type, const std::string& path, const std::string& fontName, int fontSize, int fontThick, int fontEdge)
{
	type_ = type;
	path_ = Application::PATH_FONT + path;
	fontName_ = fontName;

	fontSize_ = fontSize;
	fontThick_ = fontThick;
	fontEdge_ = fontEdge;
}

Resource::Resource(TYPE type, const std::string& path, int numX, int numY, int sizeX, int sizeY)
{
	type_ = type;
	path_ = Application::PATH_IMAGE + path;

	numX_ = numX;
	numY_ = numY;
	sizeX_ = sizeX;
	sizeY_ = sizeY;
}

Resource::Resource(TYPE type, const std::string& path, float volumeMult, bool playLoop)
{
	type_ = type;

	switch (type_)
	{
	case Resource::TYPE::MUSIC:
		path_ = Application::PATH_BGM + path;
		break;
	case Resource::TYPE::SE:
		path_ = Application::PATH_SE + path;
		break;
	}

	volumeMult_ = volumeMult;

	playType_ = DX_PLAYTYPE_BACKBIT;
	if (playLoop) playType_ += DX_PLAYTYPE_LOOPBIT;
}

Resource::Resource(TYPE type, const std::string& path, unsigned int constBufferSize)
{
	type_ = type;
	path_ = Application::PATH_SHADER + path;

	constBufferSize_ = constBufferSize;
}

Resource::~Resource()
{
}

void Resource::Load()
{
	switch (type_)
	{
	case Resource::TYPE::DXFONT:
		handleId_ = LoadFontDataToHandle(path_.c_str(), fontEdge_);
		break;
	case Resource::TYPE::EFFEKSEER:
		handleId_ = LoadEffekseerEffect(path_.c_str());
		break;
	case Resource::TYPE::FONT:
		{
			int fontType = fontEdge_ > 0 ?
				DX_FONTTYPE_ANTIALIASING_EDGE_4X4 : DX_FONTTYPE_ANTIALIASING_4X4;

			handleId_ = CreateFontToHandle(fontName_.c_str(), fontSize_, fontThick_, fontType, -1, fontEdge_);
		}
		break;
	case Resource::TYPE::IMAGE:
		handleId_ = LoadGraph(path_.c_str());
		break;
	case Resource::TYPE::IMAGES:
		handleIds_.reserve(numX_ * numY_);

		LoadDivGraph(
			path_.c_str(), numX_ * numY_, numX_, numY_, sizeX_, sizeY_, handleIds_.data());
		break;
	case Resource::TYPE::MODEL:
		handleId_ = MV1LoadModel(path_.c_str());
		break;
	case Resource::TYPE::MUSIC:
		handleId_ = LoadSoundMem(path_.c_str());
		break;
	case Resource::TYPE::PIXEL_SHADER:
		handleId_ = LoadPixelShader(path_.c_str());

		if (constBufferSize_ > 0)
			constBufferId_ = CreateShaderConstantBuffer(sizeof(FLOAT4) * constBufferSize_);
		break;
	case Resource::TYPE::SE:
		handleId_ = LoadSoundMem(path_.c_str());
		break;
	case Resource::TYPE::JSON:
		// JSONファイルを開く
		std::ifstream ifs(path_.c_str());
		if (ifs.is_open() && ifs.good())
		{
			ifs >> json_;
		}
		break;
	}
}

void Resource::Release()
{
	switch (type_)
	{
	case Resource::TYPE::DXFONT:
		DeleteFontToHandle(handleId_);
		break;
	case Resource::TYPE::EFFEKSEER:
		DeleteEffekseerEffect(handleId_);
		break;
	case Resource::TYPE::FONT:
		DeleteFontToHandle(handleId_);
		break;
	case Resource::TYPE::IMAGE:
		DeleteGraph(handleId_);
		break;
	case Resource::TYPE::IMAGES:
		{
			int num = numX_ * numY_;
			for (int i = 0; i < num; i++)
			{
				DeleteGraph(handleIds_[i]);
			}
			handleIds_.clear();
		}
		break;
	case Resource::TYPE::MODEL:
		{
			MV1DeleteModel(handleId_);
			auto& ids = duplicateModelIds_;
			for (auto id : ids)
			{
				MV1DeleteModel(id);
			}
		}
		break;
	case Resource::TYPE::JSON:
		json_.clear();
		break;
	}
}

void Resource::CopyHandle(std::vector<int>& imgs) const
{
	if (handleIds_.size() == 0)
	{
		return;
	}

	int num = numX_ * numY_;
	imgs.reserve(num);
	for (int i = 0; i < num; i++)
	{
		imgs[i] = handleIds_[i];
	}
}
