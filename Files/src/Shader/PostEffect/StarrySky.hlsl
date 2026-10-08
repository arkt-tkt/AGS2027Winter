// C++側から受け取る定数バッファ（16バイト境界）
// 定数バッファは4番目以降を使用すること（DXライブラリは0～3番目を占有しているため）
cbuffer cbParam : register(b4)
{
    float time; // 経過時間
    float resX; // 画面の幅
    float resY; // 画面の高さ
    bool scrollY; // 桁合わせ
}

struct PS_INPUT
{
    float4 svPos : SV_POSITION;
    float4 diffuse : COLOR0;
    float2 uv : TEXCOORD0;
};

// 2D座標から擬似乱数を生成する関数
float hash(float2 p)
{
    return frac(sin(dot(p, float2(12.9898, 78.233))) * 43758.5453123);
}

float4 main(PS_INPUT input) : SV_TARGET
{
    float2 uv = input.uv;
    
    // 背景のベースカラー（濃い紺色～黒の宇宙空間）
    float3 finalColor = float3(0.0, 0.0, 0.08);

    // 3つのレイヤーで視差（パララックス）を表現
    for (int i = 1; i <= 3; i++)
    {
        // レイヤーごとにスクロール速度を変える（近い星ほど速い）
        float speed = 0.03 * i;
        float2 st = uv;
        
        // ★右から左へスクロール
        // UVのX座標に時間を足すことで、テクスチャが左へ移動しているように見せる
        if (scrollY)
        {
            st.y -= time * speed;
        }
        else
        {
            st.x += time * speed;
        }
        
        // 画面の縦横比を考慮して星が横に伸びないようにする
        st.x *= (resX / resY);

        // 空間をグリッド状に分割（遠くの星ほど細かく分割）
        float scale = 15.0 * (4 - i);
        float2 grid = st * scale;
        float2 id = floor(grid);
        float2 fv = frac(grid);

        // グリッドごとに乱数を取得
        float randVal = hash(id);

        // 上位数%の確率で星を配置
        if (randVal > 0.95)
        {
            // 星の中心座標（グリッド内でランダムにずらす）
            float2 starPos = float2(hash(id + 13.0), hash(id + 37.0)) * 0.6 + 0.2;
            float dist = distance(fv, starPos);
            
            // 星の発光（ブルーム）と中心のコア（近い星ほど大きく）
            float size = 0.04 + 0.02 * i;
            float glow = smoothstep(size * 2.0, 0.0, dist);
            float core = smoothstep(size * 0.5, 0.0, dist);

            // 時間と乱数を使った瞬き（トゥインクル）
            float twinkle = sin(time * 5.0 + randVal * 100.0) * 0.5 + 0.5;
            
            // 星の色（少し青白くする）
            float3 starColor = float3(0.8, 0.9, 1.0) * (core + glow * 0.5) * twinkle;
            
            // 加算合成
            finalColor += starColor;
        }
    }

    return float4(finalColor, 1.0);
}