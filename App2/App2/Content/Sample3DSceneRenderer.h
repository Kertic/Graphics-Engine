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


	private:
		void Rotate(float radians);

	private:
		// Cached pointer to device resources.
		std::shared_ptr<DX::DeviceResources> m_deviceResources;

		// Direct3D resources for cube geometry.
		Microsoft::WRL::ComPtr<ID3D11InputLayout>	m_PlaneInputLayout;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_PlaneVertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_PlaneIndexBuffer;
		Microsoft::WRL::ComPtr<ID3D11VertexShader>	m_PlaneVertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_PlanePixelShader;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_PlaneConstantBuffer;

		Microsoft::WRL::ComPtr<ID3D11InputLayout>	m_PyramidinputLayout;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_PyramidvertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_PyramidindexBuffer;
		Microsoft::WRL::ComPtr<ID3D11VertexShader>	m_PyramidvertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_PyramidpixelShader;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_PyramidconstantBuffer;
		static const unsigned int m_PyramidCount = MAX_INSTANCES_OF_GEOMETRY;

		Microsoft::WRL::ComPtr<ID3D11InputLayout>	m_CustomMeshInputLayout;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_CustomMeshVertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_CustomMeshIndexBuffer;
		Microsoft::WRL::ComPtr<ID3D11VertexShader>	m_CustomMeshVertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_CustomMeshPixelShader;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_CustomMeshConstantBuffer;

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
	};
}

