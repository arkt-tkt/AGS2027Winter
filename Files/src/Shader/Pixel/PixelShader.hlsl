// VS/PS共通
#include "../Common/VertexToPixelHeader.hlsli"
// IN
#define PS_INPUT VertexToPixelArr
#include "../Common/Pixel/PixelShader3DHeader.hlsli"

float4 main(PS_INPUT PSInput) : SV_TARGET0
{
    float4 PSOutput;
    float3 Normal;
    float4 TextureDiffuseColor;
    float3 ShadowRate;
    float2 DepthTexCoord;
    float4 TextureDepth;
    
    Normal = normalize(PSInput.normal);
    
	// ディフューズテクスチャカラーを取得
    TextureDiffuseColor = diffuseMapTexture.Sample(diffuseMapSampler, PSInput.texCoords0_1.xy);
    
    
		// 頂点のテクスチャ座標値が範囲内の場合のみ処理する
    if (PSInput.shadowMap0Pos.x < -1.0f || PSInput.shadowMap0Pos.x > 1.0f ||
		PSInput.shadowMap0Pos.y < -1.0f || PSInput.shadowMap0Pos.y > 1.0f ||
		PSInput.shadowMap0Pos.z < 0.0f || PSInput.shadowMap0Pos.z > 1.0f)
    {
        ShadowRate.x = 1.0f;
    }
    else
    {
		// 深度テクスチャの座標を算出
		// PSInput.ShadowMap0Pos.xy は -1.0f ～ 1.0f の値なので、これを 0.0f ～ 1.0f の値にする
        DepthTexCoord.x = (PSInput.shadowMap0Pos.x + 1.0f) / 2.0f;

		// yは更に上下反転
        DepthTexCoord.y = 1.0f - (PSInput.shadowMap0Pos.y + 1.0f) / 2.0f;

		// 深度バッファテクスチャから深度を取得
        TextureDepth = shadowMap0Texture.Sample(g_ShadowMap0Sampler, DepthTexCoord);

		// テクスチャに記録されている深度( +補正値 )よりＺ値が大きかったら奥にあるということで減衰率を最大にする
        ShadowRate.x = smoothstep(PSInput.ShadowMap0Pos.z - g_ShadowMap.Data[0].GradationParam, PSInput.ShadowMap0Pos.z, TextureDepth.r + g_ShadowMap.Data[0].AdjustDepth);
    }

		// 頂点のテクスチャ座標値が範囲内の場合のみ処理する
    if (PSInput.ShadowMap1Pos.x < -1.0f || PSInput.ShadowMap1Pos.x > 1.0f ||
		    PSInput.ShadowMap1Pos.y < -1.0f || PSInput.ShadowMap1Pos.y > 1.0f ||
		    PSInput.ShadowMap1Pos.z < 0.0f || PSInput.ShadowMap1Pos.z > 1.0f)
    {
        ShadowRate.y = 1.0f;
    }
    else
    {
			// 深度テクスチャの座標を算出
			// PSInput.ShadowMap2Pos_ShadowMap3PosX.xy は -1.0f ～ 1.0f の値なので、これを 0.0f ～ 1.0f の値にする
        DepthTexCoord.x = (PSInput.ShadowMap1Pos.x + 1.0f) / 2.0f;

			// yは更に上下反転
        DepthTexCoord.y = 1.0f - (PSInput.ShadowMap1Pos.y + 1.0f) / 2.0f;

			// 深度バッファテクスチャから深度を取得
        TextureDepth = g_ShadowMap1Texture.Sample(g_ShadowMap1Sampler, DepthTexCoord);

			// テクスチャに記録されている深度( +補正値 )よりＺ値が大きかったら奥にあるということで減衰率を最大にする
        ShadowRate.y = smoothstep(PSInput.ShadowMap1Pos.z - g_ShadowMap.Data[1].GradationParam, PSInput.ShadowMap1Pos.z, TextureDepth.r + g_ShadowMap.Data[1].AdjustDepth);
    }

		// 頂点のテクスチャ座標値が範囲内の場合のみ処理する
    if (PSInput.ShadowMap2Pos.x < -1.0f || PSInput.ShadowMap2Pos.x > 1.0f ||
		    PSInput.ShadowMap2Pos.y < -1.0f || PSInput.ShadowMap2Pos.y > 1.0f ||
		    PSInput.ShadowMap2Pos.z < 0.0f || PSInput.ShadowMap2Pos.z > 1.0f)
    {
        ShadowRate.z = 1.0f;
    }
    else
    {
			// 深度テクスチャの座標を算出
			// PSInput.ShadowMap2Pos.x と PSInput.ShadowMap2Pos.y は -1.0f ～ 1.0f の値なので、これを 0.0f ～ 1.0f の値にする
        DepthTexCoord.x = (PSInput.ShadowMap2Pos.x + 1.0f) / 2.0f;

			// yは更に上下反転
        DepthTexCoord.y = 1.0f - (PSInput.ShadowMap2Pos.y + 1.0f) / 2.0f;

			// 深度バッファテクスチャから深度を取得
        TextureDepth = g_ShadowMap2Texture.Sample(g_ShadowMap2Sampler, DepthTexCoord);

			// テクスチャに記録されている深度( +補正値 )よりＺ値が大きかったら奥にあるということで減衰率を最大にする
        ShadowRate.z = smoothstep(PSInput.ShadowMap2Pos.z - g_ShadowMap.Data[2].GradationParam, PSInput.ShadowMap2Pos.z, TextureDepth.r + g_ShadowMap.Data[2].AdjustDepth);
    }
    
    return PSOutput;
}