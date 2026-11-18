Texture2D tex : register(t0);
SamplerState smp : register(s0);

cbuffer constants : register(b4)
{
    float2 dir;
    float2 texelSize;
};

static const float offset[5] = { 0.0, 1.0, 2.0, 3.0, 4.0 };
static const float weight[5] =
{
    0.2270270270,
    0.1945945946,
    0.1216216216,
    0.0540540541,
    0.0162162162
};

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float4 diffuse : COLOR0;
    float2 uv : TEXCOORD0;
};

float4 main(PS_INPUT input) : SV_TARGET
{
    float3 col = tex.Sample(smp, input.uv).rgb * weight[0];

    for (int i = 1; i < 5; i++)
    {
        float2 o = dir * offset[i] * texelSize;
        col += tex.Sample(smp, input.uv + o).rgb * weight[i];
        col += tex.Sample(smp, input.uv - o).rgb * weight[i];
    }

    return float4(col, 1.0);
}