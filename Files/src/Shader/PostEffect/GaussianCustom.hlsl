Texture2D tex : register(t0);
SamplerState smp : register(s0);

cbuffer constants : register(b4)
{
    float2 dir;
    float2 texelSize;
    float sigma;
    float count;
};

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float4 diffuse : COLOR0;
    float2 uv : TEXCOORD0;
};
/*
// 二次元ガウス関数
float gaussian2D(float x, float y, float sigma)
{
    float PI = 3.14159265f;
    return exp(-(x * x + y * y) / (2.0f * sigma * sigma)) / (2.0f * PI * sigma * sigma);
}
*/
// 一次元ガウス関数
float gaussian1D(float x, float sigma)
{
    float PI = 3.14159265f;
    return exp(-(x * x) / (2.0f * sigma * sigma)) / (sqrt(2.0f * PI) * sigma);
}

float4 main(PS_INPUT input) : SV_TARGET
{
    if (sigma == 0.0)
    {
        return tex.Sample(smp, input.uv);
    }
    
    float gauss = gaussian1D(0, sigma);
    float sum = gauss;
    float3 col = tex.Sample(smp, input.uv).rgb * gauss;
    
    [loop]
    for (int i = 1; i < (int) count; i++)
    {
        gauss = gaussian1D(i, sigma);
        float2 o = dir * i * texelSize;
        col += tex.Sample(smp, input.uv + o).rgb * gauss;
        col += tex.Sample(smp, input.uv - o).rgb * gauss;
        
        sum += gauss * 2;
    }
    col /= sum;

    return float4(col, 1.0);
}