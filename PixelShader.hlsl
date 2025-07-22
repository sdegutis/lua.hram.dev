Texture2D mytexture : register(t0);
SamplerState mysampler : register(s0);

float4 main(float2 tex : TEXCOORD0) : SV_TARGET
{
    float4 col = mytexture.Sample(mysampler, tex);
    uint norm = col.r * 0xff;
    
    uint r = (norm & 0xf0) >> 4;
    uint g = (norm & 0x0f);
    
    return float4((float) r / (float) 0xf, (float) g / (float) 0xf, 0, 1);
}
