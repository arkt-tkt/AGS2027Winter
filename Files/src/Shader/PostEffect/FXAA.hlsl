// テクスチャ
Texture2D tex : register(t0);

// サンプラー
SamplerState texSampler : register(s0);

// 定数バッファ
cbuffer cbParam : register(b4)
{
    float threshold;
    float3 unUsed;
}

struct PS_INPUT
{
    float4 svPos : SV_POSITION; // 座標
    float4 diffuse : COLOR0; // 拡散反射光
    float2 uv : TEXCOORD0; // テクスチャUV値
    float2 suv : TEXCOORD1; // サブテクスチャUV値
};

// モノトーン内積
float DotMonotone(float3 col)
{
    return dot(col, float3(0.299, 0.587, 0.114));
}

// メイン処理
float4 main(PS_INPUT PSInput) : SV_TARGET
{
    float2 uv = PSInput.uv;
    uint x, y;
    tex.GetDimensions(x, y);
    float2 px = 1.0f / float2((float) x, (float) y);
    
    // 現在のピクセル
    float4 centerCol = tex.Sample(texSampler, uv);
    float center = DotMonotone(centerCol.rgb);
    
    // 上のピクセル
    float4 upCol = tex.Sample(texSampler, float2(uv.x, uv.y - px.y));
    float up = DotMonotone(upCol.rgb);
    
    // 下のピクセル
    float4 downCol = tex.Sample(texSampler, float2(uv.x, uv.y + px.y));
    float down = DotMonotone(downCol.rgb);
    
    // 左のピクセル
    float4 leftCol = tex.Sample(texSampler, float2(uv.x - px.x, uv.y));
    float left = DotMonotone(leftCol.rgb);
    
    // 右のピクセル
    float4 rightCol = tex.Sample(texSampler, float2(uv.x + px.x, uv.y));
    float right = DotMonotone(rightCol.rgb);
    
    // 最高輝度
    float maxLuma = max(center, max(up, max(down, max(left, right))));
    // 最低輝度
    float minLuma = min(center, min(up, min(down, min(left, right))));
    
    // 最高輝度と最低輝度の差  
    if (maxLuma - minLuma >= 0.2f)
    {
        // 上下の最大輝度差
        float lumaDiffUD = max(center, max(up, down)) - min(center, min(up, down));
        // 左右の最大輝度差
        float lumaDiffLR = max(center, max(left, right)) - min(center, min(left, right));
        
        if (lumaDiffUD > lumaDiffLR)
        {
            return (centerCol + upCol + downCol) / 3.0;
        }
        else if (lumaDiffUD < lumaDiffLR)
        {
            return (centerCol + leftCol + rightCol) / 3.0;
        }
        
        return (centerCol + upCol + downCol + leftCol + rightCol) / 5.0;
    }
    
    return centerCol;
}