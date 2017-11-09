// A constant buffer that stores the three basic column-major matrices for composing geometry.
cbuffer ModelViewProjectionConstantBuffer : register(b0)
{

	matrix model[5];
	matrix view;
	matrix projection;
	float4 cameraPos;
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
	float2 uv : TEXCOORD;
	
};



// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
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

// Simple shader to do vertex processing on the GPU.
PixelShaderInput main(VertexShaderInput input, uint instanceID : SV_InstanceID)
{
	PixelShaderInput output;
	float4 pos = float4(input.pos, 1.0f);
	float4 nor = float4(input.norm, 0.0f);
	
	output.camPosition = cameraPos;

	// Transform the vertex position into projected space.
	pos = mul(pos, model[instanceID]);
	output.worldPos = pos;
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
	float3 lightnorm = float3(Lightnorm.xyz);
	output.Lightnorm = normalize(lightnorm);
	output.Lightcolor = Lightcolor;

	//Bring the light's position through spaces
	//output.Lightpos = mul(Lightpos, model[instanceID]);
	//output.Lightpos = mul(output.Lightpos, view);
	//output.Lightpos = mul(output.Lightpos, projection);
	output.Lightpos = Lightpos;
	output.Lighttype = Lighttype;
	output.Specularity = 128;

	if (instanceID == 2) {
		output.Specularity = 10;
	}
	
	output.uv = input.uv;
	return output;
}
