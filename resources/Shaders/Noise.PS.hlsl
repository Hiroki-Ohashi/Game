#include "Fullscreen.hlsli"

// 時間やフレーム数に基づくノイズ生成用定数バッファ
struct NoiseParams
{
    float time; // 時間に基づく変化
};

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);
ConstantBuffer<NoiseParams> gNoise : register(b0);

// ピクセルシェーダーの出力構造体
struct PixelShaderOutput
{
    float4 color : SV_TARGET;
};

// ランダムなノイズを生成
float random(float2 uv)
{
    return frac(sin(dot(uv.xy, float2(12.9898, 78.233))) * 43758.5453);
}

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;

    // 元のテクスチャの色をサンプリング
    float4 color = gTexture.Sample(gSampler, input.texcoord);

    // UV座標に基づいてノイズを生成
    float noise = random(input.texcoord + gNoise.time);

    // ノイズをテクスチャに適用（強さを調整）
    color.rgb += noise * 0.1f;

    output.color = color;
    return output;
}