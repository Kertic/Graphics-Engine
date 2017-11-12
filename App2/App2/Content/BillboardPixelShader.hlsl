// Per-pixel color data passed through the pixel shader.

texture2D baseTexture : register (t0);
SamplerState filter : register (s0);
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
	float lightRatio;
float3 LightDir;
float3 baseColor = float3( baseTexture.Sample(filter, input.texcoord).xyz);


	return float4(baseColor, 1.0f);
}
