// A constant buffer that stores the three basic column-major matrices for composing geometry.
cbuffer ModelViewProjectionConstantBuffer : register(b0)
{

	matrix model[5];
	matrix view;
	matrix projection;
};
cbuffer VertexShaderLightInput : register(b1) {
	float4 Lightnorm;
	float4 Lightcolor;
	float4 Lightpos;
	float4 Lighttype;
	
};
// Per-vertex data used as input to the vertex shader.
struct VertexShaderInput
{
	float3 pos : POSITION;
	float3 color : COLOR0;
	float3 norm : NORMAL;
	
};



// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float3 color : COLOR0;
	float3 normWorld : NORMAL;
	float3 normView : NORMAL2;

	float3 Lightnorm : NORMAL1;
	float3 Lightcolor : COLOR1;
	float3 Lightpos : POSITION1;
	float3 Lighttype : POSITION2;
	
};

// Simple shader to do vertex processing on the GPU.
PixelShaderInput main(VertexShaderInput input, uint instanceID : SV_InstanceID)
{
	PixelShaderInput output;
	float4 pos = float4(input.pos, 1.0f);
	float4 nor = float4(input.norm, 0.0f);
	
	// Transform the vertex position into projected space.
	pos = mul(pos, model[instanceID]);
	pos = mul(pos, view);
	pos = mul(pos, projection);
	output.pos = pos;
	

	// Pass the color through without modification.
	output.color = input.color;

	//Bring normals through spaces
	nor = normalize(nor);
	nor  = mul(nor, model[instanceID]);
	

	output.normWorld = nor;
	float3 nor2 = mul(nor, view);
	output.normView = nor2;
	output.Lightnorm = normalize(Lightnorm);
	output.Lightcolor = Lightcolor;
	output.Lightpos = Lightpos;
	output.Lighttype = Lighttype;
	
	return output;
}
