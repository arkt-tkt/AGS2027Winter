// VS/PS共通
#include "../Common/VertexToPixelHeader.hlsli"
// IN
#define PS_INPUT VertexToPixel
#include "../Common/Pixel/PixelShader3DHeader.hlsli"

float4 main(PS_INPUT PSInput) : SV_TARGET
{
    float4 PSOutput;
    float4 TextureDiffuseColor;
    float3 TotalDiffuse;
    float OutputAlpha;
    
    // ディフューズテクスチャをサンプリングする
    TextureDiffuseColor = diffuseMapTexture.Sample(diffuseMapSampler, PSInput.texCoords0_1.xy);
    
    TotalDiffuse = 0.0f;
    TotalDiffuse += g_common.material.ambientEmissive.rgb;
    
	// 出力カラー = TotalDiffuse * テクスチャカラー
    PSOutput.rgb = TotalDiffuse * TextureDiffuseColor.rgb;

	// 出力α = テクスチャα * ディフューズα * 大域α
    OutputAlpha = TextureDiffuseColor.a * g_base.factorColor.a * PSInput.diffuse.a;
    
    if (OutputAlpha < 0.01)
    {
        discard;
    }
    
	// 単純色加算
    PSOutput.rgb += g_base.drawAddColor.rgb;
    
	// アルファ乗算カラー
    if (g_base.mulAlphaColor.x > 0.5f)
    {
        PSOutput.rgb *= OutputAlpha;
    }

    PSOutput.a = OutputAlpha;
    
    return PSOutput;
}