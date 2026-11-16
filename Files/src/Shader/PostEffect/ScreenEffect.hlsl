Texture2D tex : register(t0);
SamplerState smp : register(s0);

cbuffer constants : register(b4)
{
    // ガウス
    float gaussianSigma; // ぼかし強度(標準偏差)
    int gaussianSamples; // サンプル数(-1x～1xの値を使用)
    // ビネット
    float vignetteStrength; // 強度
    float vignetteRadius; // 中心からの半径(0.0～1.0)
    float vignetteSoftness; // フェードの柔らかさ(0.0～1.0)
    float3 vignetteColor;
    // カラーオーバーレイ
    float colorOverlayStrength; // カラーオーバーレイの強度(0.0～1.0)
    float3 colorOverlayColor;
};

// ガウス関数
float gaussian(float offset, float sigma)
{
    return exp(-(offset * offset) / (2.0 * sigma * sigma));
}

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

float4 main(PS_INPUT input) : SV_TARGET
{
    float4 color = float4(0, 0, 0, 0);
    uint2 textureSize;
    tex.GetDimensions(textureSize.x, textureSize.y);
    float2 texelSize = 1.0 / textureSize;
    
    // ガウス
    if (gaussianSigma > 0.0)
    {
        // 左から右
        float weightSum = 0.0;
        for (int i = -gaussianSamples; i <= gaussianSamples; ++i)
        {
            float w = gaussian(i, gaussianSigma);
            float2 sampleUV = input.uv + float2(texelSize.x, 0) * i;
            color += tex.Sample(smp, sampleUV) * w;
            weightSum += w;
        }
        color /= weightSum;
        
        // 上から下
        weightSum = 0.0;
        for (int j = -gaussianSamples; j <= gaussianSamples; ++j)
        {
            float w = gaussian(j, gaussianSigma);
            float2 sampleUV = input.uv + float2(0, texelSize.y) * j;
            color += tex.Sample(smp, sampleUV) * w;
            weightSum += w;
        }
        color /= weightSum;
    }
    
    // ビネット
    // UV座標を中心基準に変換
    float2 centerUV = input.uv - 0.5;
    // 距離を計算(正規化)
    float dist = length(centerUV * (textureSize.xy / textureSize.x));
    // ビネット係数を計算
    float vignette = smoothstep(vignetteRadius, vignetteRadius - vignetteSoftness, dist);
    // 色を暗くする
    color.rgb *= vignette;
    
    
    // カラーオーバーレイ
    color.rgb *= lerp(float3(1.0, 1.0, 1.0), colorOverlayColor, colorOverlayStrength);
    
    
    return color;
}