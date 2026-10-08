// VS/PS共通
#include "../Common/VertexToPixelHeader.hlsli"
// IN
#include "../Common/Vertex/VertexInputType.hlsli"
#define VERTEX_INPUT DX_MV1_VERTEX_TYPE_NMAP_1FRAME
// OUT
#define VS_OUTPUT VertexToPixelArr
#include "../Common/Vertex/VertexShader3DHeader.hlsli"

cbuffer Constant : register(b4)
{
    int4 g_dummy;
}

VS_OUTPUT main(VS_INPUT VSInput)
{
    VS_OUTPUT VSOutput;
    float4 lLocalPosition;
    float4 lWorldPosition;
    float4 lViewPosition;
    float3 lWorldNorm;
    float3 lViewNorm;
    float lVerticalFogY;
    float lFogDensity;
    
    
	// 頂点座標変換 ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++( 開始 )
    
    // 座標( プロジェクション空間 )
    lLocalPosition.xyz = VSInput.pos;
    lLocalPosition.w = 1.0;
    
    lWorldPosition.xyz = mul(lLocalPosition, g_base.localWorldMatrix);
    lWorldPosition.w = 1.0;
    
    lViewPosition.xyz = mul(lWorldPosition, g_base.viewMatrix);
    lViewPosition.w = 1.0;
    
    VSOutput.position = mul(lViewPosition, g_base.projectionMatrix);
    
    // 座標( ビュー空間 )
    VSOutput.vPosition = lViewPosition.xyz;
    
    // 法線( ビュー空間 )
    lWorldNorm = mul(VSInput.Normal, (float3x3) g_base.localWorldMatrix);
    lViewNorm = mul(lWorldNorm, (float3x3) g_base.viewMatrix);
    
    VSOutput.normal = lViewNorm;
    
    // ディフューズカラー
    VSOutput.diffuse = g_base.diffuseSource > 0.5f ? VSInput.diffuse : g_common.material.diffuse;
    
    // スペキュラカラー
    VSOutput.specular = (g_base.specularSource > 0.5f ? VSInput.specular : g_common.material.specular) * g_base.mulSpecularColor;
    
	// 頂点座標変換 ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++( 終了 )
    
    
	// フォグ計算 =============================================( 開始 )
    
    VSOutput.fog.x = 1.0f;
        
    if (g_common.fog.mode == 1) // FOG_EXP
    {
		// 指数フォグ計算 1.0f / pow( e, 距離 * density )
        VSOutput.fog.x = 1.0f / pow(abs(g_common.fog.e), lViewPosition.z * g_common.fog.density);
    }
    else if (g_common.fog.mode == 2) // FOG_EXP2
    {
		// 指数フォグ２計算 1.0f / pow( e, ( 距離 * density ) * ( 距離 * density ) )
        VSOutput.fog.x = 1.0f / pow(abs(g_common.fog.e), (lViewPosition.z * g_common.fog.density) * (lViewPosition.z * g_common.fog.density));
    }
    else if (g_common.fog.mode == 3) // FOG_LINEAR
    {
        // 線形フォグ計算
        VSOutput.fog.x = lViewPosition.z * g_common.fog.linearDiv + g_common.fog.linearAdd;
    }
    
    VSOutput.fog.y = 1.0f;
        
    if (g_common.verticalFog.mode == 1 || g_common.verticalFog.mode == 2) // FOG_EXP || FOG_EXP2
    {
        if (g_common.verticalFog.density < 0.0)
        {
            lVerticalFogY = lWorldPosition.y - g_common.verticalFog.densityStart;
            lFogDensity = -g_common.verticalFog.density;
        }
        else
        {
            lVerticalFogY = g_common.verticalFog.densityStart - lWorldPosition.y;
            lFogDensity = g_common.verticalFog.density;
        }
        if (lVerticalFogY > 0.0f)
        {
            if (g_common.verticalFog.mode == 1) // FOG_EXP
            {
				// 指数フォグ計算 1.0f / pow( e, 距離 * density )
                VSOutput.fog.y = 1.0f / pow(abs(g_common.verticalFog.e), lVerticalFogY * lFogDensity);
            }
            else if (g_common.verticalFog.mode == 2) // FOG_EXP2
            {
				// 指数フォグ２計算 1.0f / pow( e, ( 距離 * density ) * ( 距離 * density ) )
                VSOutput.fog.y = 1.0f / pow(abs(g_common.verticalFog.e), (lVerticalFogY * lFogDensity) * (lVerticalFogY * lFogDensity));
            }
        }
    }
    else if (g_common.verticalFog.mode == 3) // FOG_LINEAR
    {
		// 線形フォグ計算
        VSOutput.fog.y = lWorldPosition.y * g_common.verticalFog.linearDiv + g_common.verticalFog.linearAdd;
    }
    
	// フォグ計算 =============================================( 終了 )
    
    
	// 深度影用のライトから見た射影座標を算出 =================( 開始 )
    
	    // ワールド座標をシャドウマップ０のライト設定の射影座標に変換
    VSOutput.shadowMap0Pos.x = dot(lWorldPosition, g_otherMatrix.shadowMapLightViewProjectionMatrix[0][0]);
    VSOutput.shadowMap0Pos.y = dot(lWorldPosition, g_otherMatrix.shadowMapLightViewProjectionMatrix[0][1]);
    VSOutput.shadowMap0Pos.z = dot(lWorldPosition, g_otherMatrix.shadowMapLightViewProjectionMatrix[0][2]);

	    // ワールド座標をシャドウマップ１のライト設定の射影座標に変換
    VSOutput.shadowMap1Pos.x = dot(lWorldPosition, g_otherMatrix.shadowMapLightViewProjectionMatrix[1][0]);
    VSOutput.shadowMap1Pos.y = dot(lWorldPosition, g_otherMatrix.shadowMapLightViewProjectionMatrix[1][1]);
    VSOutput.shadowMap1Pos.z = dot(lWorldPosition, g_otherMatrix.shadowMapLightViewProjectionMatrix[1][2]);

	    // ワールド座標をシャドウマップ２のライト設定の射影座標に変換
    VSOutput.shadowMap2Pos.x = dot(lWorldPosition, g_otherMatrix.shadowMapLightViewProjectionMatrix[2][0]);
    VSOutput.shadowMap2Pos.y = dot(lWorldPosition, g_otherMatrix.shadowMapLightViewProjectionMatrix[2][1]);
    VSOutput.shadowMap2Pos.z = dot(lWorldPosition, g_otherMatrix.shadowMapLightViewProjectionMatrix[2][2]);
    
	// 深度影用のライトから見た射影座標を算出 =================( 終了 )
    
    return VSOutput;
}