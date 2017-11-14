// A constant buffer that stores the three basic column-major matrices for composing geometry.
cbuffer ModelViewProjectionConstantBuffer : register(b0)
{

	matrix model[5];
	matrix view;
	matrix projection;
	float4 cameraPos;
};

// Per-vertex data used as input to the vertex shader.
struct VertexShaderInput
{
	float4 pos : POSITION0;
	float2 size : SIZE;
	float2 uv : TEXCOORD;
	
};



// Per-pixel color data passed through the pixel shader.
struct GeoShaderInput
{
	float4 pos : SV_POSITION;
	float2 size : SIZE;
	float2 texcoord : TEXCOORD;
	float4 camPosition : POSITION0;
};



// Simple shader to do vertex processing on the GPU.
GeoShaderInput main(VertexShaderInput input)
{
	GeoShaderInput output;

	
	
	// Set the output's position to the incoming position
	output.pos = input.pos;
	output.camPosition = cameraPos;
	output.size = input.size;
	output.texcoord = input.uv;

	return output;
}
