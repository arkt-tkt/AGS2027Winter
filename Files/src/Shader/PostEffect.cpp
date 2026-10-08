#include <stdexcept>
#include "PostEffect.h"

PostEffect::PostEffect(int shaderHandle, int constBufferHandle, unsigned int constBufferSize)
{
	shader_ = shaderHandle;

	cBufferHandle_ = constBufferHandle;

	cBufferSize_ = constBufferSize;
}

PostEffect::~PostEffect()
{
	cBuffer_.clear();
}

void PostEffect::MakeSquareVertex(Vector2 pos, Vector2 size)
{
	pos_ = pos;
	size_ = size;

	int cnt = 0;
	float sX = static_cast<float>(pos.x);
	float sY = static_cast<float>(pos.y);
	float eX = static_cast<float>(pos.x + size.x);
	float eY = static_cast<float>(pos.y + size.y);

	// ４頂点の初期化
	for (int i = 0; i < 4; i++)
	{
		vertexes_[i].rhw = 1.0f;
		vertexes_[i].dif = GetColorU8(255, 255, 255, 255);
		vertexes_[i].spc = GetColorU8(255, 255, 255, 255);
		vertexes_[i].su = 0.0f;
		vertexes_[i].sv = 0.0f;
	}

	// 左上
	vertexes_[cnt].pos = VGet(sX, sY, 0.0f);
	vertexes_[cnt].u = 0.0f;
	vertexes_[cnt].v = 0.0f;
	cnt++;

	// 右上
	vertexes_[cnt].pos = VGet(eX, sY, 0.0f);
	vertexes_[cnt].u = 1.0f;
	vertexes_[cnt].v = 0.0f;
	cnt++;

	// 右下
	vertexes_[cnt].pos = VGet(eX, eY, 0.0f);
	vertexes_[cnt].u = 1.0f;
	vertexes_[cnt].v = 1.0f;
	cnt++;

	// 左下
	vertexes_[cnt].pos = VGet(sX, eY, 0.0f);
	vertexes_[cnt].u = 0.0f;
	vertexes_[cnt].v = 1.0f;

	/*
	　～～～～～～
		0-----1
		|     |
		|     |
		3-----2
	　～～～～～～
		0-----1
		|  ／
		|／
		3
	　～～～～～～
			  1
		   ／ |
		 ／   |
		3-----2
	　～～～～～～
	*/

	// 頂点インデックス
	cnt = 0;
	indexes_[cnt++] = 0;
	indexes_[cnt++] = 1;
	indexes_[cnt++] = 3;

	indexes_[cnt++] = 1;
	indexes_[cnt++] = 2;
	indexes_[cnt++] = 3;
}

void PostEffect::SetUseTextures(std::vector<int> textures)
{
	textures_ = textures;
}

void PostEffect::Draw()
{
	// ピクセルシェーダ設定
	SetUsePixelShader(shader_);

	size_t size;

	// ピクセルシェーダにテクスチャを転送
	const auto& textures = textures_;
	size = textures.size();
	for (int i = 0; i < size; i++)
	{
		SetUseTextureToShader(i, textures[i]);
	}

	// 定数バッファハンドル
	int cBuffH = cBufferHandle_;

	if (cBuffH > 0)
	{
		FLOAT4* cBuffPtr = (FLOAT4*)GetBufferShaderConstantBuffer(cBuffH);
		const auto& cBuff = cBuffer_;

		size = cBuff.size();
		for (int i = 0; i < size; i++)
		{
			if (i != 0)
			{
				cBuffPtr++;
			}
			cBuffPtr->x = cBuff[i].x;
			cBuffPtr->y = cBuff[i].y;
			cBuffPtr->z = cBuff[i].z;
			cBuffPtr->w = cBuff[i].w;
		}

		// 定数バッファを更新して書き込んだ内容を反映する
		UpdateShaderConstantBuffer(cBuffH);

		// 定数バッファをピクセルシェーダー用定数バッファレジスタにセット
		SetShaderConstantBuffer(
			cBuffH, DX_SHADERTYPE_PIXEL, CBUFFER_SLOT_BEGIN);
	}

	// テクスチャアドレスタイプの取得
	int texAType = static_cast<int>(texAddress_);

	// テクスチャアドレスタイプを変更
	SetTextureAddressModeUV(texAType, texAType);

	// 描画
	DrawPolygonIndexed2DToShader(vertexes_, NUM_VERTEX, indexes_, NUM_POLYGON);

	// テクスチャアドレスタイプを元に戻す
	SetTextureAddressModeUV(DX_TEXADDRESS_CLAMP, DX_TEXADDRESS_CLAMP);

	// 後始末
	//-----------------------------------------

	// テクスチャ解除
	size = textures.size();
	for (int i = 0; i < size; i++)
	{
		SetUseTextureToShader(i, -1);
	}

	// ピクセルシェーダ解除
	SetUsePixelShader(-1);

	// オリジナルシェーダ設定(OFF)
	MV1SetUseOrigShader(false);
	//-----------------------------------------
}

void PostEffect::SetTexAddressMode(TEXADDRESS mode)
{
	texAddress_ = mode;
}

void PostEffect::AddConstantBuffer(FLOAT4 cBuffer)
{
	if (cBuffer_.size() >= cBufferSize_) return;

	cBuffer_.push_back(cBuffer);
}

int PostEffect::EditConstantBuffer(unsigned int index, FLOAT4 cBuffer)
{
	if (index > cBufferSize_) return -2;
	if (cBuffer_.size() <= index) return -1;

	cBuffer_[index] = cBuffer;
	return 0;
}
