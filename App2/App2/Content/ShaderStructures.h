#pragma once

namespace App2
{
	// Constant buffer used to send MVP matrices to the vertex shader.
	struct ModelViewProjectionConstantBuffer
	{
		DirectX::XMFLOAT4X4 model[5];
		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT4X4 projection;
		DirectX::XMFLOAT4 cameraPosition;
	};
	

	// Used to send per-vertex data to the vertex shader.
	struct VertexPositionColorNormalUV
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 color;
		DirectX::XMFLOAT3 norm;
		DirectX::XMFLOAT2 uv;

	};

	struct LightNormalColorPositionType {
		DirectX::XMFLOAT4 Lightnorm;
		DirectX::XMFLOAT4 Lightcolor;
		DirectX::XMFLOAT4 Lightpos;
		DirectX::XMFLOAT4 LightType;
	};

	struct PositionScalerUV {
		DirectX::XMFLOAT4 CloudStartPosition;
		DirectX::XMFLOAT2 XandYScale;
		DirectX::XMFLOAT2 UV;
	};

	struct HullShaderData
	{
		DirectX::XMUINT2 detailLevel;
		DirectX::XMUINT2 padding;
	};
	struct DomainShaderData
	{
		DirectX::XMFLOAT4X4 model;
		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT4X4 proj;
	};
}