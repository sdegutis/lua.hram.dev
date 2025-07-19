struct VSOUT
{
    float2 tex : TEXCOORD0;
    float4 pos : SV_POSITION;
};

VSOUT main(uint vI : SV_VERTEXID)
{
    VSOUT Out;
    Out.tex = float2(vI % 2, vI % 4 / 2);
    Out.pos = float4((Out.tex.x - 0.5f) * 2, -(Out.tex.y - 0.5f) * 2, 0, 1);
    return Out;
}
