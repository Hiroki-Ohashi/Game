#include "Fullscreen.hlsli"

//// フォグ用のパラメータ
//struct FogParams
//{
//    float4 fogColor; // フォグの色
//    float fogDensity; // フォグの濃さ
//    float fogStart; // フォグ開始距離
//    float fogEnd; // フォグ終了距離
//    int fogType; // フォグの種類
//    float nearClip; // カメラの Near Clip
//    float farClip; // カメラの Far Clip
//};

// テクスチャとサンプラーの定義
Texture2D<float4> gTexture : register(t0); // カラー用テクスチャ
Texture2D<float> gDepthTexture : register(t1); // 深度バッファ
SamplerState gSampler : register(s0);
//ConstantBuffer<FogParams> gFog : register(b0);

// シェーダーの出力構造体
struct PixelShaderOutput
{
    float4 color : SV_TARGET;
};

// 深度バッファ値をカメラ空間の奥行きに変換
float DepthToCameraDistance(float depth)
{
    float z = depth * 2.0 - 1.0;
    return 0.1f * 10000.0f / (10000.0f - z * (10000.0f - 0.1f));
}

// ピクセルシェーダー関数
PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;

    // 元のテクスチャの色を取得
    float4 texColor = gTexture.Sample(gSampler, input.texcoord);

    // 深度バッファからピクセルの深度値を取得
    float rawDepth = gDepthTexture.Sample(gSampler, input.texcoord);
    
    // 深度値をカメラ空間の距離に変換
    float depth = DepthToCameraDistance(rawDepth);

    // フォグの適用率
    float fogFactor = 0.0f;

    //// フォグの種類に応じた計算
    //if (gFog.fogType == 0)  // リニアフォグ
    //{
    //    fogFactor = saturate((50.0f - depth) / (50.0f - 10.0f));
    //}
    //else if (gFog.fogType == 1)  // 指数関数フォグ
    //{
    //    fogFactor = exp(-0.02f * depth);
    //}
    //else if (gFog.fogType == 2)  // 指数関数の二乗フォグ
    //{
    //    fogFactor = exp(-0.02f * depth * depth);
    //}
    
    fogFactor = exp(-0.02f * depth * depth);

    // フォグを適用（フォグの色とテクスチャの色をブレンド）
    output.color = lerp((1.0f, 0.75f, 0.8f, 1.0f), texColor, fogFactor);

    return output;
}
