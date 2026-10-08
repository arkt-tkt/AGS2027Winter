#pragma once
#include <DxLib.h>
#include <string>
#include <vector>
#include "../Common/Geometry.h"

class PostEffect
{
public:
	enum class TEXADDRESS
	{
		NONE = 0,
		WRAP,
		MIRROR,
		CLAMP,
		BORDER,
		MAX
	};

	PostEffect(int shaderHandle, int constBufferHandle, unsigned int constBufferSize);
	~PostEffect();
	void MakeSquareVertex(Vector2 pos, Vector2 size);
	void SetUseTextures(std::vector<int> textures);

	void Draw();

	void SetTexAddressMode(TEXADDRESS mode);
	void AddConstantBuffer(FLOAT4 cBuffer);
	int EditConstantBuffer(unsigned int index, FLOAT4 cBuffer);

private:
	static constexpr int CBUFFER_SLOT_BEGIN = 4;
	static constexpr int NUM_VERTEX = 4;
	static constexpr int NUM_VERTEX_IDX = 6;
	static constexpr int NUM_POLYGON = 2;

	int shader_;

	int cBufferHandle_;
	unsigned int cBufferSize_;
	std::vector<FLOAT4> cBuffer_;

	Vector2 pos_;
	Vector2 size_;
	std::vector<int> textures_;
	TEXADDRESS texAddress_ = TEXADDRESS::CLAMP;

	VERTEX2DSHADER vertexes_[NUM_VERTEX];
	WORD indexes_[NUM_VERTEX_IDX];

};