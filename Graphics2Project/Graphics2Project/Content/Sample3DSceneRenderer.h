#pragma once

#include "..\Common\DeviceResources.h"
#include "ShaderStructures.h"
#include "..\Common\StepTimer.h"

namespace Graphics2Project
{
	// This sample renderer instantiates a basic rendering pipeline.
	class Sample3DSceneRenderer
	{
	public:
		Sample3DSceneRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources);
		void CreateDeviceDependentResources();
		
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
		Microsoft::WRL::ComPtr<ID3D11InputLayout>	m_CubeInputLayout;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_CubeVertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_CubeIndexBuffer;
		Microsoft::WRL::ComPtr<ID3D11VertexShader>	m_CubeVertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_CubePixelShader;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_CubeConstantBuffer;

		Microsoft::WRL::ComPtr<ID3D11InputLayout>	m_PyramidinputLayout;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_PyramidvertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_PyramidindexBuffer;
		Microsoft::WRL::ComPtr<ID3D11VertexShader>	m_PyramidvertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_PyramidpixelShader;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_PyramidconstantBuffer;

		// System resources for cube geometry.
		ModelViewProjectionConstantBuffer	m_CubeConstantBufferData;
		uint32	m_indexCount;

		ModelViewProjectionConstantBuffer	m_PyramidconstantBufferData;
		uint32	m_PyramidindexCount;

		// Variables used with the rendering loop.
		bool	m_loadingComplete;
		float	m_degreesPerSecond;
		bool	m_tracking;
	};
}

