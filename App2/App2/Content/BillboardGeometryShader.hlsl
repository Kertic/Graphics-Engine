
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
void BillboardShader(point GeoShaderInput points[1] : SV_POSITION, inout TriangleStream<GeoShaderInput> triStream) {




	float3 startPos = float3(points[0].pos.xyz);

	//Now we build billboard pieces
	float3 up = float3(0.0f, 1.0f, 0.0f);
	float3 look = points[0].camPosition.xyz - points[0].pos.xyz;
	look.y = 0.0f;//Locking it along the y axis, can be undone for full facing
	look = normalize(look);
	float3 right = cross(up, look);

	float halfWidth = 0.5f * points[0].size.x;//Make our horizontal radius (but for rectangles)
	float halfHeight = 0.5f * points[0].size.y;//Vertical radius


	GeoShaderInput verts[4] =
	{
		float4(startPos + halfWidth*right - halfHeight * up, 1.0f), float2(points[0].size.x, points[0].size.y), float2(points[0].texcoord.x, 1.0f), points[0].camPosition,
		float4(startPos + halfWidth*right + halfHeight * up, 1.0f),float2(points[0].size.x, points[0].size.y), float2(points[0].texcoord.x, points[0].texcoord.y), points[0].camPosition,

		float4(startPos - halfWidth*right - halfHeight * up, 1.0f), float2(points[0].size.x, points[0].size.y), float2(1.0f, 1.0f), points[0].camPosition,
		float4(startPos - halfWidth*right + halfHeight * up, 1.0f), float2(points[0].size.x, points[0].size.y), float2(1.0f, points[0].texcoord.y), points[0].camPosition
	};




	float4x4 viewProj = mul(view, projection);
	for (uint i = 0; i < 4; ++i) {
		verts[i].pos = mul(verts[i].pos, viewProj);
	}


	triStream.Append(verts[3]);
	triStream.Append(verts[1]);
	triStream.Append(verts[2]);
	triStream.Append(verts[0]);
}