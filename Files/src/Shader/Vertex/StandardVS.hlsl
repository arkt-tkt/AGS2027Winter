// VS/PS共通
#include "../Common/VertexToPixelHeader.hlsli"
// IN
#include "../Common/Vertex/VertexInputType.hlsli"
#define VERTEX_INPUT DX_MV1_VERTEX_TYPE_NMAP_1FRAME
// OUT
#define VS_OUTPUT VertexToPixel
#include "../Common/Vertex/VertexShader3DHeader.hlsli"

VS_OUTPUT main(VS_INPUT VSInput)
{
    VS_OUTPUT VSOutput;
    float4 lLocalPosition;
    float4 lWorldPosition;
    float4 lViewPosition;
    float3 lWorldNorm;
    float3 lViewNorm;
    
    // 座標（プロジェクション空間）
    lLocalPosition.xyz = VSInput.pos;
    lLocalPosition.w = 1.0;
    
    lWorldPosition.xyz = mul(lLocalPosition, g_base.localWorldMatrix);
    lWorldPosition.w = 1.0;
    
    lViewPosition.xyz = mul(lWorldPosition, g_base.viewMatrix);
    lViewPosition.w = 1.0;
    
    VSOutput.position = mul(lViewPosition, g_base.projectionMatrix);
    
    // 座標（ビュー空間）
    VSOutput.vPosition = lViewPosition.xyz;
    
    // 法線（ビュー空間）
    lWorldNorm = mul(VSInput.norm, (float3x3) g_base.localWorldMatrix);
    lViewNorm = mul(lWorldNorm, (float3x3) g_base.viewMatrix);
    
    VSOutput.normal = lViewNorm;
    
    // ディフューズカラー
    VSOutput.diffuse = g_base.diffuseSource > 0.5f ? VSInput.diffuse : g_common.material.diffuse;
    
    // スペキュラカラー
    VSOutput.specular = (g_base.specularSource > 0.5f ? VSInput.specular : g_common.material.specular) * g_base.mulSpecularColor;
    
    return VSOutput;
}