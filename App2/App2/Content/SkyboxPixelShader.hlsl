// Per-pixel color data passed through the pixel shader.

textureCUBE baseTexture : register(t2);
SamplerState filter : register(s0);
struct PixelShaderInput
{

    float4 pointsForGeoShader : POSITION5;
    float2 geoShaderUv : TEXCOORD1;
    float4 pos : SV_POSITION;
    float3 worldPos : POSITION3;
    float3 color : COLOR0;
    float3 normWorld : NORMAL;
    float3 normView : NORMAL2;
    float2 uv : TEXCOORD;

    float4 camPosition : POSITION4;

    float3 Lightnorm : NORMAL1;
    float3 Lightcolor : COLOR1;
    float3 Lightpos : POSITION1;
    float3 Lighttype : POSITION2;
    float Specularity : COLOR2;


};

// A pass-through function for the (interpolated) color data.
float4 main(PixelShaderInput input) : SV_TARGET
{

    float3 baseColor = baseTexture.Sample(filter, input.worldPos);
    input.color = baseColor;
  


    return float4(input.color, 1.0f);
}
