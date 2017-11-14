
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



[maxvertexcount(8)]
void BillboardShader(point GeoShaderInput points[1] : SV_POSITION, inout TriangleStream<GeoShaderInput> triStream)
{




    float3 startPos = float3(mul(points[0].pos, model[0] ).xyz);

	//Now we build billboard pieces
    float3 up = float3(0.0f, 1.0f, 0.0f);
    float3 look = points[0].camPosition.xyz - startPos;
    look.y = 0.0f; //Locking it along the y axis, can be undone for full facing
    look = normalize(look);
    float3 right = cross(up, look);

    float halfWidth = 0.5f * points[0].size.x; //Make our horizontal radius (but for rectangles)
    float halfHeight = 0.5f * points[0].size.y; //Vertical radius


    float4 newLocation = points[0].pos;
    newLocation.x += 2.0f;
    float3 startPos2 = float3(mul(newLocation, model[0] ).xyz);
    startPos2.x += 2.0f;
    float3 newCamPos = points[0].camPosition.xyz;
   // newCamPos.x += 4.0f;
	//Now we build billboard pieces
    float3 up2 = float3(0.0f, 1.0f, 0.0f);
    float3 look2 = newCamPos - startPos2;
    look2.y = 0.0f; //Locking it along the y axis, can be undone for full facing
    look2 = normalize(look2);
    float3 right2 = cross(up2, look2);

    float halfWidth2 = 0.5f * points[0].size.x; //Make our horizontal radius (but for rectangles)
    float halfHeight2 = 0.5f * points[0].size.y; //Vertical radius


    GeoShaderInput verts[4] =
    {
        float4(startPos + halfWidth * right - halfHeight * up, 1.0f), float2(points[0].size.x, points[0].size.y), float2(points[0].texcoord.x, 1.0f), points[0].camPosition,
		float4(startPos + halfWidth * right + halfHeight * up, 1.0f), float2(points[0].size.x, points[0].size.y), float2(points[0].texcoord.x, points[0].texcoord.y), points[0].camPosition,

		float4(startPos - halfWidth * right - halfHeight * up, 1.0f), float2(points[0].size.x, points[0].size.y), float2(1.0f, 1.0f), points[0].camPosition,
		float4(startPos - halfWidth * right + halfHeight * up, 1.0f), float2(points[0].size.x, points[0].size.y), float2(1.0f, points[0].texcoord.y), points[0].camPosition
    };

    GeoShaderInput verts2[4] =
    {
        float4(startPos2 + halfWidth2 * right2 - halfHeight2 * up2, 1.0f), float2(points[0].size.x, points[0].size.y), float2(points[0].texcoord.x, 1.0f), points[0].camPosition,
		float4(startPos2 + halfWidth2 * right2 + halfHeight2 * up2, 1.0f), float2(points[0].size.x, points[0].size.y), float2(points[0].texcoord.x, points[0].texcoord.y), points[0].camPosition,

		float4(startPos2 - halfWidth2 * right2 - halfHeight2 * up2, 1.0f), float2(points[0].size.x, points[0].size.y), float2(1.0f, 1.0f), points[0].camPosition,
		float4(startPos2 - halfWidth2 * right2 + halfHeight2 * up2, 1.0f), float2(points[0].size.x, points[0].size.y), float2(1.0f, points[0].texcoord.y), points[0].camPosition
    };


    float4x4 viewProj = mul( view, projection);
    for (uint i = 0; i < 4; ++i)
    {
        verts[i].pos = mul(verts[i].pos, viewProj);
    }
    for (uint j = 0; j < 4; ++j)
    {
        verts2[j].pos = mul(verts2[j].pos, viewProj);
    }


    triStream.Append(verts[3]);
    triStream.Append(verts[1]);
    triStream.Append(verts[2]);
    triStream.Append(verts[0]);

    triStream.RestartStrip();
    triStream.Append(verts2[3]);
    triStream.Append(verts2[1]);
    triStream.Append(verts2[2]);
    triStream.Append(verts2[0]);

}