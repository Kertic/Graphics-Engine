// Per-pixel color data passed through the pixel shader.

Texture2D baseTexture : register(t0);
Texture2D multiTex : register(t1);
SamplerState filter : register(s0);
struct PixelShaderInput
{
    float4 pos : SV_POSITION;
    float2 size : SIZE;
    float2 texcoord : TEXCOORD;
    float4 camPosition : POSITION0;


};

// A pass-through function for the (interpolated) color data.
float4 main(PixelShaderInput input) : SV_TARGET
{

    float4 baseColor = float4(baseTexture.Sample(filter, input.texcoord));
    float4 multiColor = float4(multiTex.Sample(filter, input.texcoord));
    multiColor = multiColor * baseColor * multiColor.w;
    if (baseColor.w < 0.75f)
        discard;
    if (multiColor.w != 0.0f)
        baseColor = multiColor;
    if (baseColor.w < 0.75f)
        discard;
    


    return float4(baseColor);
}
