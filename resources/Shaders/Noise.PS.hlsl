#include "Fullscreen.hlsli"

// 時間やフレーム数に基づくノイズ生成用定数バッファ
struct NoiseParams
{
    float time; // 時間に基づく変化
    float lineStrength;
    float noiseStrength;
    float vignetteLight;
    float vignetteShape;
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

    // ノイズ処理
    // UV座標のy成分に基づいた横ラインノイズの生成
    float lineNoise = sin(input.texcoord.y * 500.0f + gNoise.time) * gNoise.lineStrength;

    // ランダムノイズの追加
    float randomNoise = random(input.texcoord + gNoise.time);

    // 横ラインノイズとランダムノイズを結合
    float noise = lineNoise + randomNoise * gNoise.noiseStrength;

    // ノイズをテクスチャに適用（強さを調整）
    color.rgb += noise * 0.1f;

    // ビネット効果の適用
    float2 correct = input.texcoord * (1.0f - input.texcoord.yx);
    float vignette = correct.x * correct.y * gNoise.vignetteLight;

    // ビネットの強度調整
    vignette = saturate(pow(vignette, gNoise.vignetteShape)); // 0.0fから0.5fに変更してビネットの形状を調整

    // ビネットを適用
    color.rgb *= vignette;

    output.color = color;
    return output;
}