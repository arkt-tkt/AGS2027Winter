/*
struct VertexToPixel
{
    float4 svPos		: SV_POSITION;
    float4 diffuse		: COLOR0;
    float4 specular		: COLOR1;
    float2 uv			: TEXCOORD;
};

struct VertexToPixelLit
{
	float4 svPos		: SV_POSITION;	// 座標( プロジェクション空間 )
	float2 uv			: TEXCOORD0;	// テクスチャ座標
	float3 vwPos		: TEXCOORD1;	// 座標( ビュー座標 )
	float3 normal       : TEXCOORD2;	// 法線( ビュー座標 )
	float4 diffuse      : COLOR0;		// ディフューズカラー
	float3 lightDir     : TEXCOORD3;	// ライト方向(ローカル)
	float3 lightAtPos   : TEXCOORD4;	// ライトから見た座標
};

struct VertexToPixelShadow
{
	float4 svPos		: SV_POSITION;	// 座標( プロジェクション空間 )
	float2 uv			: TEXCOORD0;	// テクスチャ座標
	float4 vwPos		: TEXCOORD1;	// 座標( ビュー座標 )
};
*/

struct VertexToPixel
{
    float4 diffuse		: COLOR0;		// ディフューズカラー
    float4 specular		: COLOR1;		// スペキュラカラー
    float4 texCoords0_1	: TEXCOORD0;	// xy:テクスチャ座標 zw:サブテクスチャ座標
    float3 vPosition	: TEXCOORD1;	// 座標( ビュー空間 )
    float3 normal		: TEXCOORD2;	// 法線( ビュー空間 )
	float3 vTan			: TEXCOORD3;	// 接線( ビュー空間 )
	float3 vBin			: TEXCOORD4;	// 従法線( ビュー空間 )
    float2 fog			: TEXCOORD5;	// フォグパラメータ( x )   高さフォグパラメータ( y )
	float3 shadowMap0Pos: TEXCOORD6;	// シャドウマップ０のライト座標( x, y, z )
	float3 shadowMap1Pos: TEXCOORD7;	// シャドウマップ１のライト座標( x, y, z )
    float3 shadowMap2Pos: TEXCOORD8;	// シャドウマップ２のライト座標( x, y, z )
    float4 position		: SV_POSITION;	// 座標( プロジェクション空間 )
};