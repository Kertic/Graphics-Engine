



cbuffer HullShaderData : register(b0)
{
    uint2 detailLevel;
    uint2 filler;
};
// A constant buffer that stores the three basic column-major matrices for composing geometry.
cbuffer DomainShaderData : register(b1)
{

    matrix model;
    matrix view;
    matrix projection;
   
};

// Input control point
struct VS_CONTROL_POINT_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 size : SIZE;
    float2 texcoord : TEXCOORD;
    float4 camPosition : POSITION0;
};

// Output control point
struct HS_CONTROL_POINT_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 size : SIZE;
    float2 texcoord : TEXCOORD;
    float4 camPosition : POSITION0;
    float4x4 model : MODEL;
    float4x4 view : VIEW;
    float4x4 proj : PROJ;
};


[domain("tri")] // tessellator builds triangles
[partitioning("fractional_odd")] // type of division
[outputtopology("triangle_cw")] // topology of output
[outputcontrolpoints(3)] // num CP sent to Domain Shader
// points to HLSL function which computes tessellation amount
[patchconstantfunc("CalcHSPatchConstants")]
HS_CONTROL_POINT_OUTPUT main(
	InputPatch<VS_CONTROL_POINT_OUTPUT, 3> ip,
	uint i : SV_OutputControlPointID,
	uint PatchID : SV_PrimitiveID)
{
    HS_CONTROL_POINT_OUTPUT Output;
	// Direct transfer of control points to Domain Shader 1:1
    Output.pos = ip[i].pos;
    Output.size = ip[i].size;
    Output.texcoord = ip[i].texcoord;
    Output.camPosition = ip[i].camPosition;
    Output.model = model;
    Output.view = view;
    Output.proj = projection;
    return Output;
}


// Output patch constant data.(One for each patch)
struct HS_CONSTANT_DATA_OUTPUT
{
	// [3] edges on a triangle, would be [4] for a quad domain.
    float EdgeTessFactor[3] : SV_TessFactor;
	// would be InsideTessFactor[2] for a quad domain.
    float InsideTessFactor : SV_InsideTessFactor;
};
// Patch Constant Function
HS_CONSTANT_DATA_OUTPUT CalcHSPatchConstants(
	InputPatch<VS_CONTROL_POINT_OUTPUT, 3> ip,
	uint PatchID : SV_PrimitiveID)
{
    HS_CONSTANT_DATA_OUTPUT Output;
	// Set interior & edge tessellation factors
    Output.EdgeTessFactor[0] =
	Output.EdgeTessFactor[1] =
	Output.EdgeTessFactor[2] =
	Output.InsideTessFactor = detailLevel.x; // ~8 partitions
    return Output;
}
