#include "Fullscreen.hlsli"

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
	
};

struct GaussianParam
{
    float32_t stepWidth;
    float32_t sigma;
};

ConstantBuffer<GaussianParam> gGaussian : register(b0);

Texture2D<float32_t4> gTexture : register(t0);

SamplerState gSampler : register(s0);

float32_t Gaussian(float32_t2 drawUV, float32_t2 pickUV, float32_t sigma)
{
    float32_t d = distance(drawUV, pickUV);
    return exp(-(d * d) / (2 * sigma * sigma));
}

float32_t4 WeightCalc(float32_t2 texCoord)
{
    float32_t totalWeight = 0;
    float32_t sigma = gGaussian.sigma;
    float32_t stepWidth = gGaussian.stepWidth;
    float32_t4 col = float32_t4(0, 0, 0, 0);
    
    for (float32_t py = -sigma * 2; py <= sigma * 2; py += stepWidth)
    {
        for (float32_t px = -sigma * 2; px <= sigma * 2; px += stepWidth)
        {
            float32_t2 pickUV = texCoord + float32_t2(px, py);
            float32_t weight = Gaussian(texCoord, pickUV, sigma);
            float32_t4 color = gTexture.Sample(gSampler, pickUV);
            col += color * weight;
            totalWeight += weight;
        }

    }
    
    col.rgb = col.rgb / totalWeight;
    col.a = 1.0f;
    return col;
}

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
   
    // ガウシアンブラー
    float32_t4 gaussian = WeightCalc(input.texcoord.xy);

    output.color = gaussian;
    return output;
}