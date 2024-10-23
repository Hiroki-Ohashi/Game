#include "Fullscreen.hlsli"

// ラジアルブラーの設定
struct RadialBlurParams
{
    float blurStrength; // ブラーの強度（放射状にサンプリングする距離）
    int sampleCount; // サンプリング数（サンプルを取る回数）
};

// テクスチャとサンプラーを定義
Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);
ConstantBuffer<RadialBlurParams> gBlur : register(b0);

// シェーダーの出力構造体
struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

// ピクセルシェーダー関数
PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;

    // 現在のピクセルのテクスチャ座標
    float2 texcoord = input.texcoord;
    
    // 中心点から現在のピクセルへのベクトルを計算
    float2 direction = texcoord - float2(0.5, 0.5);

    // 初期の色（最初のサンプル）
    float32_t4 resultColor = gTexture.Sample(gSampler, texcoord);
    
    // サンプルの合計
    float totalWeight = 1.0;

    // ラジアルブラー処理：中心に向かってサンプリング
    for (int i = 1; i <= gBlur.sampleCount; ++i)
    {
        // サンプル位置を計算（中心に向かう方向に少しずつ近づける）
        float sampleFactor = i / (float) gBlur.sampleCount * gBlur.blurStrength;
        float2 sampleCoord = texcoord - direction * sampleFactor;

        // サンプリングした色を取得
        float32_t4 sampledColor = gTexture.Sample(gSampler, sampleCoord);
        
        // 重み付けを計算（距離に基づいて）
        float weight = 1.0 / (i + 1);
        
        // 色を加算して重みで調整
        resultColor += sampledColor * weight;
        totalWeight += weight;
    }

    // 平均を取って最終的な色を出力
    output.color = resultColor / totalWeight;

    return output;
}