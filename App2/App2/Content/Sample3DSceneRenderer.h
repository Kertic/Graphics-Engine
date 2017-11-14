#pragma once

#include "..\Common\DeviceResources.h"
#include "ShaderStructures.h"
#include "..\Common\StepTimer.h"
#include "defines.h"

#include <DirectXMath.h>
using namespace DirectX;

namespace App2
{
	// This sample renderer instantiates a basic rendering pipeline.
	class Sample3DSceneRenderer
	{
	public:
		Sample3DSceneRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources);
		void CreateDeviceDependentResources();
		void CreateCustomMesh();
		void CreateWindowSizeDependentResources();
		void ReleaseDeviceDependentResources();
		void Update(DX::StepTimer const& timer);
		void Render();
		void StartTracking();
		void TrackingUpdate(float positionX);
		void StopTracking();
		bool IsTracking() { return m_tracking; }
		void Sample3DSceneRenderer::UpdateCamera(DX::StepTimer const& timer, float const moveSpd, float const rotSpd);


	private:
		void Rotate(float radians);

	private:
		// Cached pointer to device resources.
		std::shared_ptr<DX::DeviceResources>		m_deviceResources;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>	m_RasterizerState;
		
		// Direct3D resources for cube geometry.
		Microsoft::WRL::ComPtr<ID3D11InputLayout>	m_PlaneInputLayout;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_PlaneVertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_PlaneIndexBuffer;
		Microsoft::WRL::ComPtr<ID3D11VertexShader>	m_PlaneVertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_PlanePixelShader;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_PlaneConstantBuffer;

		// Direct3D resources for cube geometry.
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_CubeVertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_CubeIndexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_CubeConstantBuffer;
	


		Microsoft::WRL::ComPtr<ID3D11InputLayout>	m_PyramidinputLayout;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_PyramidvertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_PyramidindexBuffer;
		Microsoft::WRL::ComPtr<ID3D11VertexShader>	m_PyramidvertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_PyramidpixelShader;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_PyramidconstantBuffer;
		static const unsigned int m_PyramidCount = MAX_INSTANCES_OF_GEOMETRY;
Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	m_SkyboxTexture;

		Microsoft::WRL::ComPtr<ID3D11InputLayout>	m_CustomMeshInputLayout;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_CustomMeshVertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_CustomMeshIndexBuffer;
		Microsoft::WRL::ComPtr<ID3D11VertexShader>	m_CustomMeshVertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_CustomMeshPixelShader;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_CustomMeshConstantBuffer;
		//Microsoft::WRL::ComPtr<ID3D11Texture2D>		m_CustomMeshResource;
Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	m_CustomMeshShaderResourceView;
		Microsoft::WRL::ComPtr<ID3D11SamplerState>	m_CustomMeshSamplerState;


		//Resources for billboard clouds
		Microsoft::WRL::ComPtr<ID3D11InputLayout>	m_CloudInputLayout;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_BillboardPixelShader;
		Microsoft::WRL::ComPtr<ID3D11VertexShader>	m_BillboardVertexShader;
		Microsoft::WRL::ComPtr<ID3D11GeometryShader>m_BillboardGeometryShader;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_CloudVertexBuffer;
Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	m_CloudShaderResourceView;
		Microsoft::WRL::ComPtr<ID3D11SamplerState>	m_CloudSamplerState;
		static const unsigned int m_CloudCount = 1;
		PositionScalerUV m_CloudPoints;

		//Resources for tessellated terrain
Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	m_TerrainShaderResourceView;
		Microsoft::WRL::ComPtr<ID3D11HullShader>	m_TerrianHullShader;
		Microsoft::WRL::ComPtr<ID3D11DomainShader>	m_TerrianDomainShader;
		unsigned int LevelOfDetail = 1;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_TerrainHullShaderConstantBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_TerrainDomainShaderConstantBuffer;
		HullShaderData m_TerrainConstantBufferData;
		DomainShaderData m_TerrainDomainShaderConstantBufferData;


		// System resources for cube geometry.
		ModelViewProjectionConstantBuffer	m_PlaneConstantBufferData;
		uint32	m_PlaneIndexCount;

		ModelViewProjectionConstantBuffer	m_PyramidconstantBufferData;
		uint32	m_PyramidindexCount;

		ModelViewProjectionConstantBuffer	m_CustomMeshConstantBufferData;
		uint32	m_CustomMeshIndexCount;

		// Variables used with the rendering loop.
		bool	m_loadingComplete;
		float	m_degreesPerSecond;
		bool	m_tracking;

		XMFLOAT4X4 world, camera, proj;

		enum CurrentLightingOptions
		{
			DIRECTIONAL_LIGHTING = 0,
			POINT_LIGHTING = 1,
			SPOT_LIGHTING = 2,
			NUM_LIGHTS
		};

		//Lighting variables
		Microsoft::WRL::ComPtr<ID3D11Buffer> LightConstantBuffer;

		CurrentLightingOptions currentLight;

		LightNormalColorPositionType DirectionalLight;
		LightNormalColorPositionType PointLight;
		LightNormalColorPositionType SpotLight;
		float light_DynamicOffset;
		//Camera vars
		XMVECTOR m_eye = { 0.0f, 0.0f, -1.5f, 0.0f };
		XMVECTOR m_at = { 0.0f, 1.0f, 0.0f, 0.0f };
		XMVECTOR m_up = { 0.0f, 1.0f, 0.0f, 0.0f };
	};
}

