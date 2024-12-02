#include "Particle.hlsli"

struct ParticleForGpu {
    float32_t4x4 WVP;
    float32_t4x4 World;
    float32_t4 color;
};

StructuredBuffer<ParticleForGpu> gParticle : register(t0);

struct VertexShaderInput {
    float32_t4 position : POSITION0;
    float32_t2 texcoord : TEXCOORD0;
    float32_t3 normal : NORMAL0;
};

VertexShaderOutput main(VertexShaderInput input, uint32_t instanceId : SV_InstanceID) {
    VertexShaderOutput output;
   // 座標変換
    output.position = mul(input.position, gParticle[instanceId].WVP);

    // テクスチャ座標
    output.texcoord = input.texcoord;

    // 色の設定
    output.color = gParticle[instanceId].color;
    return output;
}