
cbuffer ModelViewProjectionConstantBuffer : register(b0)
{

	matrix model[5];
	matrix view;
	matrix projection;
	float4 cameraPos;
};
//pixel shader inputs
struct GeoShaderInput
{
	float4 pos : SV_POSITION;
	float2 size : SIZE;
	float2 texcoord : TEXCOORD;
	float4 camPosition : POSITION0;
};



[maxvertexcount(4)]
void BillboardShader( point GeoShaderInput points[1] : SV_POSITION,  inout TriangleStream<GeoShaderInput> triStream) {
	GeoShaderInput verts[4] =
	{
		float4(points[0].pos.x, points[0].pos.y + (1.0f * -points[0].size.y), points[0].pos.z, 1.0f), float2(points[0].size.x, points[0].size.y), float2(points[0].texcoord.x, 1.0f), points[0].camPosition,
		float4(points[0].pos.x, points[0].pos.y, points[0].pos.z, 1.0f),float2(points[0].size.x, points[0].size.y), float2(points[0].texcoord.x, points[0].texcoord.y), points[0].camPosition,

		float4(points[0].pos.x + (1.0f * points[0].size.x), points[0].pos.y + (1.0f * -points[0].size.y), points[0].pos.z, 1.0f), float2(points[0].size.x, points[0].size.y), float2(1.0f, 1.0f), points[0].camPosition,
		float4(points[0].pos.x + (1.0f * points[0].size.x), points[0].pos.y, points[0].pos.z, 1.0f), float2(points[0].size.x, points[0].size.y), float2(1.0f, points[0].texcoord.y), points[0].camPosition
	};

	//GS_OUTPUT verts[4] =
	//{
	//	float4(pos.x, pos.y + (1.0f * -size.y), pos.z, 1.0f), float2(texcoord.x, 1.0f), points[0].pos, points[0].worldPos, points[0].color, points[0].normWorld, points[0].normView, float2(texcoord.x, 1.0f), points[0].camPosition, points[0].Lightnorm, points[0].Lightcolor, points[0].Lightpos, float3(4.0f, 0.0f, 0.0f), points[0].Specularity,
	//	float4(pos.x, pos.y, pos.z, 1.0f), float2(texcoord.x, texcoord.y), points[1].pos, points[1].worldPos, points[1].color, points[1].normWorld, points[1].normView, float2(texcoord.x, texcoord.y), points[1].camPosition, points[1].Lightnorm, points[1].Lightcolor, points[1].Lightpos, float3(4.0f, 0.0f, 0.0f), points[1].Specularity,

	//	float4(pos.x + (1.0f * size.x), pos.y + (1.0f * -size.y), pos.z, 1.0f), float2(1.0f, 1.0f), points[2].pos, points[2].worldPos, points[2].color, points[2].normWorld, points[2].normView, float2(1.0f, 1.0f), points[2].camPosition, points[2].Lightnorm, points[2].Lightcolor, points[2].Lightpos, float3(4.0f, 0.0f, 0.0f), points[2].Specularity,
	//	float4(pos.x + (1.0f * size.x), pos.y, pos.z, 1.0f), float2(1.0f, texcoord.y), points[0].pos, points[0].worldPos, points[0].color, points[0].normWorld, points[0].normView, float2(1.0f, texcoord.y), points[0].camPosition, points[0].Lightnorm, points[0].Lightcolor, points[0].Lightpos, float3(4.0f, 0.0f, 0.0f), points[0].Specularity
	//};


	float4x4 viewProj = mul(view, projection);
	for (uint i = 0; i < 4; ++i) {
		verts[i].pos = mul(verts[i].pos, viewProj);
	}


	triStream.Append(verts[0]);
	triStream.Append(verts[1]);
	triStream.Append(verts[2]);
	triStream.Append(verts[3]);
}