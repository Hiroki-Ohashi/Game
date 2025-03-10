#include "Fullscreen.hlsli"

// ノイズとブラーの設定をまとめた定数バッファ
struct PostEffectParams
{
    float time; // 時間に基づく変化
    float lineStrength; // 横ラインノイズの強度
    float noiseStrength; // ランダムノイズの強度
    float vignetteLight; // ビネットの明るさ
    float vignetteShape; // ビネットの形状
    float blurStrength; // ラジアルブラーの強度
    int sampleCount; // ブラーのサンプリング数
    float fogStart; // フォグ開始地点
    float fogDensity; // フォグの濃さ
};

// テクスチャとサンプラーの定義
Texture2D<float4> gTexture : register(t0);
Texture2D<float> gDepthTexture : register(t1);
SamplerState gSampler : register(s0);
ConstantBuffer<PostEffectParams> gParams : register(b0);

// シェーダーの出力構造体
struct PixelShaderOutput
{
    float4 color : SV_TARGET;
};

// ランダムなノイズを生成
float random(float2 uv)
{
    return frac(sin(dot(uv.xy, float2(12.9898, 78.233))) * 43758.5453);
}

// 深度バッファ値をカメラ空間の奥行きに変換
float DepthToCameraDistance(float depth)
{
    float z = depth * 2.0f - 1.0f;
    return 0.1f * 1000000.0f / (1000000.0f - z * (1000000.0f - 0.1f));
}

// ピクセルシェーダー関数
PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;

    // 元のテクスチャの色をサンプリング
    float4 color = gTexture.Sample(gSampler, input.texcoord);

    // --- フォグ処理 ---
    float rawDepth = gDepthTexture.Sample(gSampler, input.texcoord).r; // .r でチャンネル指定
    float depth = DepthToCameraDistance(rawDepth);

    // Exponential fog (指数フォグ)
    float fogFactor = exp(-gParams.fogDensity * (depth - gParams.fogStart) * 0.01f);
    fogFactor = clamp(fogFactor, 0.0, 1.0);

    float4 fogColor = float4(0.5f, 0.5f, 1.0f, 1.0f);
    
    color = lerp(fogColor, color, fogFactor);
    
    // --- ノイズ処理 ---
    
    // 横ラインノイズの生成
    float lineNoise = sin(input.texcoord.y * 500.0f + gParams.time) * gParams.lineStrength;

    // ランダムノイズの生成
    float randomNoise = random(input.texcoord + gParams.time);

    // ノイズを結合して強度を調整
    float noise = lineNoise + randomNoise * gParams.noiseStrength;
    
    // ノイズを色に適用
    color.rgb += noise * 0.1f;

    // --- ラジアルブラー処理 ---
    
    // 現在のピクセルのテクスチャ座標
    float2 texcoord = input.texcoord;
    
    // 中心点から現在のピクセルへのベクトルを計算
    float2 direction = texcoord - float2(0.5, 0.5);

    // ラジアルブラー処理：中心に向かってサンプリング
    float4 resultColor = color; // 初期の色（最初のサンプル）
    float totalWeight = 0.1;

    for (int i = 1; i <= gParams.sampleCount; ++i)
    {
        // サンプル位置を計算（中心に向かう方向に少しずつ近づける）
        float sampleFactor = i / (float) gParams.sampleCount * gParams.blurStrength;
        float2 sampleCoord = texcoord - direction * sampleFactor;

        // サンプリングした色を取得
        float4 sampledColor = gTexture.Sample(gSampler, sampleCoord);
        
        // 重み付けを計算（距離に基づいて）
        float weight = 1.0 / (i + 1);
        
        // 色を加算して重みで調整
        resultColor += sampledColor * weight;
        totalWeight += weight;
    }

    // 平均を取って最終的な色を出力
    resultColor /= totalWeight;

    // --- ビネット効果をラジアルブラー後に適用 ---
    
    // ビネット効果の計算
    float2 correct = input.texcoord * (1.0f - input.texcoord.yx);
    float vignette = correct.x * correct.y * gParams.vignetteLight;

    // ビネットの強度を調整
    vignette = saturate(pow(vignette, gParams.vignetteShape));

    // ビネットを結果色に適用
    resultColor.rgb *= vignette;
    output.color = resultColor;
    return output;
}