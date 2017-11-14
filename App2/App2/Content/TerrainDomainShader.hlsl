
texture2D baseTexture : register(t0);
SamplerState filter : register(s0);

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

    //Interpolate the UV's based on the domain ratios
    Output.texcoord = float2(patch[0].texcoord * domain.x + patch[1].texcoord * domain.y + patch[2].texcoord * domain.z);
    //Interpolate the positions based on the domain ratios
    Output.pos = float4(patch[0].pos.xyz * domain.x + patch[1].pos.xyz * domain.y + patch[2].pos.xyz * domain.z, 1);
	

    float4 height = baseTexture.SampleLevel(filter, Output.texcoord, 0); //We use y later on because, no matter what the format for colors is, the 2nd channel will always be a color and never alpha (and our height map is black and white)
   
    
    //Set the height based on one of the color channels, but they're all B/W so any color channel would work
    Output.pos.y = height.y * 5.0f;


 
    Output.pos = mul(Output.pos, patch[0].model);
    Output.pos = mul(Output.pos, patch[0].view);
    Output.pos = mul(Output.pos, patch[0].proj);

    Output.camPosition = patch[0].camPosition;
    Output.size = patch[0].size;
    return Output;
}
