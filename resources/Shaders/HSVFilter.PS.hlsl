#include "Fullscreen.hlsli"
//#include <algorithem>
//#include <cmath>

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput{
    float32_t4 color : SV_TARGET0;
};

float32_t3_hsv RGBToHSV(float32_t3 rgb) {
    float r = rgb.r;
    float g = rgb.g;
    float b = rgb.b;
    
    float max = std::max({r, g, b});
    float min = std::min({r, g, b});
    float h, s, v;
    
    float delta = max - min;
    
    // Hue calculation
    if (delta > 0.0f) {
        if (max == r) {
            h = (g - b) / delta;
            if (h < 0.0f) {
                h += 6.0f;
            }
        } else if (max == g) {
            h = 2.0f + (b - r) / delta;
        } else {
            h = 4.0f + (r - g) / delta;
        }
        h /= 6.0f;
    } else {
        h = 0.0f;
    }
    
    // Saturation calculation
    if (max > 0.0f) {
        s = delta / max;
    } else {
        s = 0.0f;
    }
    
    // Value calculation
    v = max;
    
    return float32_t3 {h, s, v};
}

float32_t3_rgb HSVToRGB(float32_t3 hsv) {
    float h = hsv.h;
    float s = hsv.s;
    float v = hsv.v;
    
    float r, g, b;
    if (s == 0.0f) {
        // Achromatic (grey)
        r = g = b = v;
    } else {
        h *= 6.0f; // sector 0 to 5
        int i = static_cast<int>(std::floor(h));
        float f = h - i; // fractional part of h
        float p = v * (1.0f - s);
        float q = v * (1.0f - s * f);
        float t = v * (1.0f - s * (1.0f - f));
        
        switch (i) {
            case 0:
                r = v;
                g = t;
                b = p;
                break;
            case 1:
                r = q;
                g = v;
                b = p;
                break;
            case 2:
                r = p;
                g = v;
                b = t;
                break;
            case 3:
                r = p;
                g = q;
                b = v;
                break;
            case 4:
                r = t;
                g = p;
                b = v;
                break;
            case 5:
            default:
                r = v;
                g = p;
                b = q;
                break;
        }
    }
    
    return float32_t3 {r, g, b};
}

PixelShaderOutput main(VertexShaderOutput input) {
    PixelShaderOutput output;
    float32_t4 textureColor = gTexture.Sample(gSampler, input.texcoord);
    float32_t3 hsv = RGBToHSV(textureColor.rgb);

    // ここで調整

    float32_t3 rgb = HSVToRGB(hsv);

    output.color.rgb = rgb;
    output.color.a = textureColor.a;
    return output;
}