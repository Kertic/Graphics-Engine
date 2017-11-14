
texture2D baseTexture : register(t0);


// Output patch constant data.(One for each patch)
struct HS_CONSTANT_DATA_OUTPUT
{
	// [3] edges on a triangle, would be [4] for a quad domain.
    float EdgeTessFactor[3] : SV_TessFactor;
	// would be InsideTessFactor[2] for a quad domain.
    float InsideTessFactor : SV_InsideTessFactor;
};

// Output from Domain Shader
struct DS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 size : SIZE;
    float2 texcoord : TEXCOORD;
    float4 camPosition : POSITION0;

};

// Output control point
struct HS_CONTROL_POINT_OUTPUT
{
   // float4 clr : COLOR; // CP color
   //// send local space to Domain
   // float3 PositionL : POSITION;

    float4 pos : SV_POSITION;
    float2 size : SIZE;
    float2 texcoord : TEXCOORD;
    float4 camPosition : POSITION0;
    float4x4 model : MODEL;
    float4x4 view : VIEW;
    float4x4 proj : PROJ;
};

[domain("tri")] // must match Hull Shader domain
DS_OUTPUT main(
	HS_CONSTANT_DATA_OUTPUT input,
	float3 domain : SV_DomainLocation,
	const OutputPatch<HS_CONTROL_POINT_OUTPUT, 3> patch)
{

    DS_OUTPUT Output;

    Output.texcoord = float2(patch[0].texcoord * domain.x + patch[1].texcoord * domain.y + patch[2].texcoord * domain.z);
	// domain location used to manually interpolate newly created geometry across overall triangle
	//The y location is generated from the texture slot's information
    uint3 sampleCoord = uint3(Output.texcoord.x, Output.texcoord.y, 0);
    float4 height = baseTexture.Load(sampleCoord); //We use y later on because, no matter what the format for colors is, the 2nd channel will always be a color and never alpha (and our height map is black and white)
    Output.pos = float4(
		patch[0].pos.xyz * domain.x + patch[1].pos.xyz * (domain.y + (height.y * 5.0f)) + patch[2].pos.xyz * domain.z, 1);
	// same deal for color data...

	// Prepare adjusted outgoing vertex for rasterization 

    matrix tempView = patch[0].view;
    matrix tempProj = patch[0].proj;
    Output.pos = mul(Output.pos, tempView);
    Output.pos = mul(Output.pos, tempProj);

    Output.camPosition = patch[0].camPosition;
    Output.size = patch[0].size;
    return Output;
}
