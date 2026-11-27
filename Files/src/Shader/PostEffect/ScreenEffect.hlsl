Texture2D tex : register(t0);
SamplerState smp : register(s0);

cbuffer constants : register(b4)
{
    float vignetteStrength; // ビネット強度
    float vignetteRadius; // ビネットの中心からの半径(0.0～1.0)
    float vignetteSoftness; // ビネットのフェードの柔らかさ(0.0～1.0)
    float dummy1;
    
    float3 vignetteColor; // ビネットの色味
    float dummy2;
    
    // カラーオーバーレイ
    float colorOverlayStrength; // カラーオーバーレイの強度(0.0～1.0)
    float3 colorOverlayColor;
};

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float4 diffuse : COLOR0;
    float2 uv : TEXCOORD0;
};

float4 main(PS_INPUT input) : SV_TARGET
{
    float4 color = tex.Sample(smp, input.uv);
    
    // ビネット
    // UV座標を中心基準に変換
    float2 centerUV = input.uv - 0.5;
    // 距離を計算(正規化)
    float dist = length(centerUV);
    // ビネット係数を計算
    float vignette = smoothstep(vignetteRadius, vignetteRadius - vignetteSoftness, dist);
    // 色を暗くする
    color.rgb *= lerp(vignetteColor, float3(1.0, 1.0, 1.0), vignette * vignetteStrength);
    
    // カラーオーバーレイ
    color.rgb *= lerp(float3(1.0, 1.0, 1.0), colorOverlayColor, colorOverlayStrength);
    
    return color;
}