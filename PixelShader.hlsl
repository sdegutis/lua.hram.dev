Texture2D mytexture : register(t0);
SamplerState mysampler : register(s0);

float4 main(float2 tex : TEXCOORD0) : SV_TARGET
{
    return mytexture.Sample(mysampler, tex);
}
