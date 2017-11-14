#include "pch.h"
#include "Sample3DSceneRenderer.h"
#include "Mesh.h"
#include "Content\DDSTextureLoader.h"

#include "..\Common\DirectXHelper.h"

using namespace App2;

using namespace DirectX;
using namespace Windows::Foundation;

// Loads vertex and pixel shaders from files and instantiates the cube geometry.
Sample3DSceneRenderer::Sample3DSceneRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
	m_loadingComplete(false),
	m_degreesPerSecond(45),
	m_PlaneIndexCount(0),
	m_tracking(false),
	m_deviceResources(deviceResources)
{
	currentLight = DIRECTIONAL_LIGHTING;
	CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();

	//XMStoreFloat4x4(&view, XMMatrixIdentity());

	DirectionalLight = {
		XMFLOAT4(0.0f, -0.5f, 0.5f, 1.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		XMFLOAT4((float)CurrentLightingOptions::DIRECTIONAL_LIGHTING, 0.0f, 0.0f, 0.0f) };

	PointLight = {
		XMFLOAT4(0.0f, -0.5f, 0.5f, 1.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		XMFLOAT4(0.0f, -1.0f, 0.0f, 1.0f),
		XMFLOAT4((float)CurrentLightingOptions::POINT_LIGHTING, 0.0f, 0.0f, 0.0f) };

	SpotLight = {
		XMFLOAT4(0.0f, -0.5f, 0.0f, 1.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f),
		XMFLOAT4((float)CurrentLightingOptions::SPOT_LIGHTING, 0.0f, 0.0f, 0.0f) };
	light_DynamicOffset = 0.0f;

}

// Initializes view parameters when the window size changes.
void Sample3DSceneRenderer::CreateWindowSizeDependentResources()
{
	Size outputSize = m_deviceResources->GetOutputSize();
	float aspectRatio = outputSize.Width / outputSize.Height;
	float fovAngleY = 60.0f * XM_PI / 180.0f;

	// This is a simple example of change that can be made when the app is in
	// portrait or snapped view.
	if (aspectRatio < 1.0f)
	{
		fovAngleY *= 2.0f;
	}

	// Note that the OrientationTransform3D matrix is post-multiplied here
	// in order to correctly orient the scene to match the display orientation.
	// This post-multiplication step is required for any draw calls that are
	// made to the swap chain render target. For draw calls to other targets,
	// this transform should not be applied.

	// This sample makes use of a right-handed coordinate system using row-major matrices.
	XMMATRIX perspectiveMatrix = XMMatrixPerspectiveFovRH(
		fovAngleY,
		aspectRatio,
		0.01f,
		100.0f
	);

	XMFLOAT4X4 orientation = m_deviceResources->GetOrientationTransform3D();

	XMMATRIX orientationMatrix = XMLoadFloat4x4(&orientation);

	//Cube
	XMStoreFloat4x4(
		&m_PlaneConstantBufferData.projection,
		XMMatrixTranspose(perspectiveMatrix * orientationMatrix)
	);

	//Pyramid

	XMStoreFloat4x4(
		&m_PyramidconstantBufferData.projection,
		XMMatrixTranspose(perspectiveMatrix * orientationMatrix)
	);

	//Custom Mesh
	XMStoreFloat4x4(
		&m_CustomMeshConstantBufferData.projection,
		XMMatrixTranspose(perspectiveMatrix * orientationMatrix)
	);

	// Eye is at (0,0.7,1.5), looking at point (0,-0.1,0) with the up-vector along the y-axis.
	static const XMVECTOR eye = { 0.0f, 0.0f, -1.5f, 0.0f };
	static const XMVECTOR at = { 0.0f, 1.0f, 0.0f, 0.0f };
	static const XMVECTOR up = { 0.0f, 1.0f, 0.0f, 0.0f };
	m_eye = eye;
	m_at = at;
	m_up = up;

	XMStoreFloat4x4(&camera, XMMatrixLookAtLH(eye, at, up));
	XMStoreFloat4x4(&m_PlaneConstantBufferData.view, XMMatrixTranspose(XMMatrixInverse(0, XMMatrixLookAtLH(eye, at, up))));
	XMStoreFloat4x4(&m_CustomMeshConstantBufferData.view, XMMatrixTranspose(XMMatrixInverse(0, XMMatrixLookAtLH(eye, at, up))));
	XMStoreFloat4x4(&m_PyramidconstantBufferData.view, XMMatrixTranspose(XMMatrixInverse(0, XMMatrixLookAtLH(eye, at, up))));
	m_TerrainConstantBufferData.detailLevel = XMUINT2(1, 0);

	XMMATRIX newcamera = XMLoadFloat4x4(&camera);
	XMMATRIX tempCam = XMMatrixInverse(0, newcamera);
	XMVECTOR pos;// = newcamera.r[3];
	XMVECTOR rot;
	XMVECTOR scale;
	XMMatrixDecompose(&scale, &rot, &pos, newcamera);
	XMStoreFloat4(&m_PlaneConstantBufferData.cameraPosition, pos);
	XMStoreFloat4(&m_PyramidconstantBufferData.cameraPosition, pos);
	XMStoreFloat4(&m_CustomMeshConstantBufferData.cameraPosition, pos);


}

using namespace Windows::UI::Core;
extern CoreWindow^ gwindow;
#include <atomic>
extern bool mouse_move;
extern float diffx;
extern float diffy;
extern bool w_down;
extern bool a_down;
extern bool s_down;
extern bool d_down;
extern bool left_click;

extern char buttons[256];

// Called once per frame, rotates the cube and calculates the model and view matrices.
void Sample3DSceneRenderer::Update(DX::StepTimer const& timer)
{
	if (!m_tracking)
	{
		// Convert degrees to radians, then convert seconds to rotation angle
		float radiansPerSecond = XMConvertToRadians(m_degreesPerSecond);
		double totalRotation = timer.GetTotalSeconds() * radiansPerSecond;
		float radians = static_cast<float>(fmod(totalRotation, XM_2PI));

		Rotate(radians);
	}



	if (buttons['1']) {
		currentLight = DIRECTIONAL_LIGHTING;
	}
	if (buttons['2']) {
		currentLight = POINT_LIGHTING;
	}
	if (buttons['3']) {
		currentLight = SPOT_LIGHTING;
	}
	if (buttons['4']) {
		light_DynamicOffset += 0.05;
	}
	if (buttons['5']) {
		light_DynamicOffset -= 0.05;
	}
	if (buttons['6']) {
		light_DynamicOffset = 0.0f;
	}
	if (buttons['7']) {
		LevelOfDetail++;
		if (LevelOfDetail > 64)
			LevelOfDetail = 64;
	}
	if (buttons['8']) {
		LevelOfDetail--;
		if (LevelOfDetail < 1)
			LevelOfDetail = 1;
	}
	if (buttons['9']) {
		D3D11_RASTERIZER_DESC rasterDesc;
		rasterDesc.AntialiasedLineEnable = false;
		rasterDesc.CullMode = D3D11_CULL_BACK;
		rasterDesc.DepthBias = 0;
		rasterDesc.DepthBiasClamp = 0.0f;
		rasterDesc.DepthClipEnable = true;
		rasterDesc.FillMode = D3D11_FILL_SOLID;
		rasterDesc.FrontCounterClockwise = false;
		rasterDesc.MultisampleEnable = false;
		rasterDesc.ScissorEnable = false;
		rasterDesc.SlopeScaledDepthBias = 0.0f;
		m_deviceResources->GetD3DDevice()->CreateRasterizerState(&rasterDesc, m_RasterizerState.GetAddressOf());
		m_deviceResources->GetD3DDeviceContext()->RSSetState(m_RasterizerState.Get());
	}
	if (buttons['0']) {
		D3D11_RASTERIZER_DESC rasterDesc;
		rasterDesc.AntialiasedLineEnable = false;
		rasterDesc.CullMode = D3D11_CULL_BACK;
		rasterDesc.DepthBias = 0;
		rasterDesc.DepthBiasClamp = 0.0f;
		rasterDesc.DepthClipEnable = true;
		rasterDesc.FillMode = D3D11_FILL_WIREFRAME;
		rasterDesc.FrontCounterClockwise = false;
		rasterDesc.MultisampleEnable = false;
		rasterDesc.ScissorEnable = false;
		rasterDesc.SlopeScaledDepthBias = 0.0f;
		m_deviceResources->GetD3DDevice()->CreateRasterizerState(&rasterDesc, m_RasterizerState.GetAddressOf());
		m_deviceResources->GetD3DDeviceContext()->RSSetState(m_RasterizerState.Get());
	}
	m_TerrainConstantBufferData.detailLevel = XMUINT2(LevelOfDetail, 0);

	Windows::UI::Input::PointerPoint^ point = nullptr;

	//if(mouse_move)/*This crashes unless a mouse event actually happened*/
		//point = Windows::UI::Input::PointerPoint::GetCurrentPoint(pointerID);



#pragma region Update Lights
	DirectionalLight = {
		XMFLOAT4(0.0f, -0.5f + light_DynamicOffset, 0.5f + light_DynamicOffset, 1.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		XMFLOAT4((float)CurrentLightingOptions::DIRECTIONAL_LIGHTING, 0.0f, 0.0f, 0.0f) };

	PointLight = {
		XMFLOAT4(0.0f, -0.5f, 0.5f + light_DynamicOffset, 1.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f , 1.0f),
		XMFLOAT4(0.0f, -1.0f + light_DynamicOffset, 0.0f, 1.0f),
		XMFLOAT4((float)CurrentLightingOptions::POINT_LIGHTING, 0.0f, 0.0f, 0.0f) };

	SpotLight = {
		XMFLOAT4(0.0f, -0.5f , 0.0f + light_DynamicOffset, 1.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		XMFLOAT4(0.0f, 1.0f + light_DynamicOffset, 0.0f, 1.0f),
		XMFLOAT4((float)CurrentLightingOptions::SPOT_LIGHTING, 0.0f, 0.0f, 0.0f) };
#pragma endregion
#pragma region Animate clouds
	if ((int)timer.GetTotalSeconds() % 2 == 0) {
		CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), L"Assets/Cloud1.dds", nullptr, &m_CloudShaderResourceView);
	}
	else
	{
		CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), L"Assets/Cloud2.dds", nullptr, &m_CloudShaderResourceView);
	}
#pragma endregion


	UpdateCamera(timer, 5.0f, 0.75f);
	XMMATRIX newcamera = XMLoadFloat4x4(&camera);


	/*Be sure to inverse the camera & Transpose because they don't use pragma pack row major in shaders*/
	XMStoreFloat4x4(&m_PlaneConstantBufferData.view, XMMatrixTranspose(XMMatrixInverse(0, newcamera)));
	XMStoreFloat4x4(&m_PyramidconstantBufferData.view, XMMatrixTranspose(XMMatrixInverse(0, newcamera)));
	XMStoreFloat4x4(&m_CustomMeshConstantBufferData.view, XMMatrixTranspose(XMMatrixInverse(0, newcamera)));
	m_TerrainDomainShaderConstantBufferData.model = m_PlaneConstantBufferData.model[0];
	m_TerrainDomainShaderConstantBufferData.view = m_PlaneConstantBufferData.view;
	m_TerrainDomainShaderConstantBufferData.proj = m_PlaneConstantBufferData.projection;


	XMVECTOR pos;// = newcamera.r[3];
	XMVECTOR rot;
	XMVECTOR scale;
	XMMatrixDecompose(&scale, &rot, &pos, newcamera);


	XMStoreFloat4(&m_PlaneConstantBufferData.cameraPosition, pos);
	XMStoreFloat4(&m_PyramidconstantBufferData.cameraPosition, pos);
	XMStoreFloat4(&m_CustomMeshConstantBufferData.cameraPosition, pos);

	mouse_move = false;/*Reset*/
}
void Sample3DSceneRenderer::UpdateCamera(DX::StepTimer const& timer, float const moveSpd, float const rotSpd)
{
	const float delta_time = (float)timer.GetElapsedSeconds();

	if (buttons['W'])
	{
		XMMATRIX translation = XMMatrixTranslation(0.0f, 0.0f, -moveSpd * delta_time);


		XMMATRIX temp_camera = XMLoadFloat4x4(&camera);
		XMMATRIX result = XMMatrixMultiply(translation, temp_camera);
		XMStoreFloat4x4(&camera, result);
	}
	if (buttons['S'])
	{
		XMMATRIX translation = XMMatrixTranslation(0.0f, 0.0f, moveSpd * delta_time);

		XMMATRIX temp_camera = XMLoadFloat4x4(&camera);
		XMMATRIX result = XMMatrixMultiply(translation, temp_camera);
		XMStoreFloat4x4(&camera, result);
	}
	if (buttons['A'])
	{
		XMMATRIX translation = XMMatrixTranslation(-moveSpd * delta_time, 0.0f, 0.0f);

		XMMATRIX temp_camera = XMLoadFloat4x4(&camera);
		XMMATRIX result = XMMatrixMultiply(translation, temp_camera);
		XMStoreFloat4x4(&camera, result);
	}
	if (buttons['D'])
	{
		XMMATRIX translation = XMMatrixTranslation(moveSpd * delta_time, 0.0f, 0.0f);

		XMMATRIX temp_camera = XMLoadFloat4x4(&camera);
		XMMATRIX result = XMMatrixMultiply(translation, temp_camera);
		XMStoreFloat4x4(&camera, result);
	}
	if (buttons['X'])
	{
		XMMATRIX translation = XMMatrixTranslation(0.0f, -moveSpd * delta_time, 0.0f);

		XMMATRIX temp_camera = XMLoadFloat4x4(&camera);
		XMMATRIX result = XMMatrixMultiply(translation, temp_camera);
		XMStoreFloat4x4(&camera, result);
	}
	if (buttons[VK_SPACE])
	{
		XMMATRIX translation = XMMatrixTranslation(0.0f, moveSpd * delta_time, 0.0f);

		XMMATRIX temp_camera = XMLoadFloat4x4(&camera);
		XMMATRIX result = XMMatrixMultiply(translation, temp_camera);
		XMStoreFloat4x4(&camera, result);
	}

	if (mouse_move)
	{
		if (left_click)
		{
			float dx = -diffx;
			float dy = -diffy;
			XMFLOAT4 pos = XMFLOAT4(camera._41, camera._42, camera._43, camera._44);

			camera._41 = 0;
			camera._42 = 0;
			camera._43 = 0;

			XMMATRIX rotX = XMMatrixRotationX(dy * rotSpd * delta_time);
			XMMATRIX rotY = XMMatrixRotationY(dx * rotSpd * delta_time);

			XMMATRIX temp_camera = XMLoadFloat4x4(&camera);
			temp_camera = XMMatrixMultiply(rotX, temp_camera);
			temp_camera = XMMatrixMultiply(temp_camera, rotY);

			XMStoreFloat4x4(&camera, temp_camera);

			camera._41 = pos.x;
			camera._42 = pos.y;
			camera._43 = pos.z;
		}

	}


}

// Rotate the 3D cube model a set amount of radians.
void Sample3DSceneRenderer::Rotate(float radians)
{
	// Prepare to pass the updated model matrix to the shader
	XMStoreFloat4x4(&m_PlaneConstantBufferData.model[0], XMMatrixTranspose(XMMatrixRotationY(0.0f)));

#pragma region Independant Pyramid Transformations
	XMStoreFloat4x4(&m_PyramidconstantBufferData.model[0], XMMatrixTranspose(XMMatrixRotationY(-radians)));
	//XMStoreFloat4x4(&m_PyramidconstantBufferData.model[1], XMMatrixTranspose(XMMatrixRotationY(radians)));

	XMStoreFloat4x4(
		&m_PyramidconstantBufferData.model[1],
		XMMatrixMultiply(
			XMMatrixTranspose(XMMatrixTranslation(1.0f, 0.0f, 0.0f)),
			XMMatrixTranspose(XMMatrixRotationY(radians))
		)
	);

	XMStoreFloat4x4(&m_PyramidconstantBufferData.model[2], XMMatrixTranspose(XMMatrixTranslation(-1.25f, 0.0f, 0.0f)));
	XMStoreFloat4x4(&m_PyramidconstantBufferData.model[3], XMMatrixTranspose(XMMatrixTranslation(-1.25f, -1.25f, 0.0f)));
	XMStoreFloat4x4(&m_PyramidconstantBufferData.model[4], XMMatrixTranspose(XMMatrixTranslation(-1.25f, -1.25f, -1.25f)));
#pragma endregion

#pragma region Independant Pyramid Transformations
	XMStoreFloat4x4(&m_CustomMeshConstantBufferData.model[0], XMMatrixTranspose(XMMatrixRotationY(-radians)));
	//XMStoreFloat4x4(&m_PyramidconstantBufferData.model[1], XMMatrixTranspose(XMMatrixRotationY(radians)));

	XMStoreFloat4x4(
		&m_CustomMeshConstantBufferData.model[1],
		XMMatrixMultiply(
			XMMatrixTranspose(XMMatrixTranslation(1.0f, 0.0f, 0.0f)),
			XMMatrixTranspose(XMMatrixRotationY(radians))
		)
	);

	XMStoreFloat4x4(&m_CustomMeshConstantBufferData.model[2], XMMatrixTranspose(XMMatrixTranslation(-1.25f, 0.0f, 0.0f)));
	XMStoreFloat4x4(&m_CustomMeshConstantBufferData.model[3], XMMatrixTranspose(XMMatrixTranslation(-1.25f, -1.25f, 0.0f)));
	XMStoreFloat4x4(&m_CustomMeshConstantBufferData.model[4], XMMatrixTranspose(XMMatrixTranslation(-1.25f, -1.25f, -1.25f)));
#pragma endregion
}

void Sample3DSceneRenderer::StartTracking()
{
	m_tracking = true;
}

// When tracking, the 3D cube can be rotated around its Y axis by tracking pointer position relative to the output screen width.
void Sample3DSceneRenderer::TrackingUpdate(float positionX)
{
	if (m_tracking)
	{
		float radians = XM_2PI * 2.0f * positionX / m_deviceResources->GetOutputSize().Width;
		Rotate(radians);
	}
}

void Sample3DSceneRenderer::StopTracking()
{
	m_tracking = false;
}

// Renders one frame using the vertex and pixel shaders.
void Sample3DSceneRenderer::Render()
{
	// Loading is asynchronous. Only draw geometry after it's loaded.
	if (!m_loadingComplete)
	{
		return;
	}

	auto context = m_deviceResources->GetD3DDeviceContext();



	//#pragma region Draw Pyramid
	//
	//
	//
	//	// Prepare the constant buffer to send it to the graphics device.
	//	context->UpdateSubresource1(
	//		m_PyramidconstantBuffer.Get(),
	//		0,
	//		NULL,
	//		&m_PyramidconstantBufferData,
	//		0,
	//		0,
	//		0
	//	);
	//
	//	// Each vertex is one instance of the VertexPositionColorNormalUV struct.
	//	stride = sizeof(VertexPositionColorNormalUV);
	//	offset = 0;
	//	context->IASetVertexBuffers(
	//		0,
	//		1,
	//		m_PyramidvertexBuffer.GetAddressOf(),
	//		&stride,
	//		&offset
	//	);
	//
	//	context->IASetIndexBuffer(
	//		m_PyramidindexBuffer.Get(),
	//		DXGI_FORMAT_R16_UINT, // Each index is one 16-bit unsigned integer (short).
	//		0
	//	);
	//
	//	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//
	//	context->IASetInputLayout(m_PyramidinputLayout.Get());
	//
	//	// Attach our vertex shader.
	//	context->VSSetShader(
	//		m_PyramidvertexShader.Get(),
	//		nullptr,
	//		0
	//	);
	//
	//	// Send the constant buffer to the graphics device.
	//	context->VSSetConstantBuffers1(
	//		0,
	//		1,
	//		m_PyramidconstantBuffer.GetAddressOf(),
	//		nullptr,
	//		nullptr
	//	);
	//
	//	// Attach our pixel shader.
	//	context->PSSetShader(
	//		m_PyramidpixelShader.Get(),
	//		nullptr,
	//		0
	//	);
	//
	//	// Draw the objects.
	//	//context->DrawIndexed(m_PyramidindexCount, 0, 0);
	//	context->DrawIndexedInstanced(m_PyramidindexCount, m_PyramidCount, 0, 0, 0);
	//
	//#pragma endregion

#pragma region Draw Custom Mesh



// Prepare the constant buffer to send it to the graphics device.
	context->UpdateSubresource1(
		m_CustomMeshConstantBuffer.Get(),
		0,
		NULL,
		&m_CustomMeshConstantBufferData,
		0,
		0,
		0
	);

	// Each vertex is one instance of the VertexPositionColorNormalUV struct.
	UINT stride = sizeof(VertexPositionColorNormalUV);
	UINT offset = 0;
	context->IASetVertexBuffers(
		0,
		1,
		m_CustomMeshVertexBuffer.GetAddressOf(),
		&stride,
		&offset
	);

	context->IASetIndexBuffer(
		m_CustomMeshIndexBuffer.Get(),
		DXGI_FORMAT_R16_UINT, // Each index is one 16-bit unsigned integer (short).
		0
	);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->IASetInputLayout(m_CustomMeshInputLayout.Get());

	// Attach our vertex shader.
	context->VSSetShader(
		m_CustomMeshVertexShader.Get(),
		nullptr,
		0
	);

	// Send the constant buffer to the graphics device.
	context->VSSetConstantBuffers1(
		0,
		1,
		m_CustomMeshConstantBuffer.GetAddressOf(),
		nullptr,
		nullptr
	);

	// Attach our pixel shader.
	context->PSSetShader(
		m_CustomMeshPixelShader.Get(),
		nullptr,
		0
	);
	context->PSSetShaderResources(0, 1, m_CustomMeshShaderResourceView.GetAddressOf());
	context->PSSetSamplers(0, 1, m_CustomMeshSamplerState.GetAddressOf());

	// Draw the objects.
	context->DrawIndexedInstanced(m_CustomMeshIndexCount, MAX_INSTANCES_OF_GEOMETRY, 0, 0, 0);

#pragma endregion
#pragma region Draw Plane



	// Prepare the constant buffer to send it to the graphics device.
	context->UpdateSubresource1(
		m_PlaneConstantBuffer.Get(),
		0,
		NULL,
		&m_PlaneConstantBufferData,
		0,
		0,
		0
	);
	context->UpdateSubresource1(
		m_TerrainHullShaderConstantBuffer.Get(),
		0,
		NULL,
		&m_TerrainConstantBufferData,
		0,
		0,
		0
	);
	context->UpdateSubresource1(
		m_TerrainDomainShaderConstantBuffer.Get(),
		0,
		NULL,
		&m_TerrainDomainShaderConstantBufferData,
		0,
		0,
		0
	);

	// Each vertex is one instance of the VertexPositionColorNormalUV struct.
	stride = sizeof(PositionScalerUV);
	offset = 0;
	context->IASetVertexBuffers(
		0,
		1,
		m_PlaneVertexBuffer.GetAddressOf(),
		&stride,
		&offset
	);

	context->IASetIndexBuffer(
		m_PlaneIndexBuffer.Get(),
		DXGI_FORMAT_R16_UINT, // Each index is one 16-bit unsigned integer (short).
		0
	);

	context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
	

	context->IASetInputLayout(m_PlaneInputLayout.Get());

	// Attach our vertex shader.
	context->VSSetShader(
		m_BillboardVertexShader.Get(),
		nullptr,
		0
	);

	// Send the constant buffer to the graphics device.
	context->VSSetConstantBuffers1(
		0,
		1,
		m_PlaneConstantBuffer.GetAddressOf(),
		nullptr,
		nullptr
	);

	// Attach our pixel shader.
	context->PSSetShader(
		m_BillboardPixelShader.Get(),
		nullptr,
		0
	);
	//Use the CustomMesh b/c this has no texture
	context->PSSetShaderResources(0, 1, m_CustomMeshShaderResourceView.GetAddressOf());
	context->PSSetSamplers(0, 1, m_CustomMeshSamplerState.GetAddressOf());
	context->HSSetShader(m_TerrianHullShader.Get(), nullptr, 0);

	context->HSSetConstantBuffers1(
		0,
		1,
		m_TerrainHullShaderConstantBuffer.GetAddressOf(),
		nullptr,
		nullptr
	);
	context->HSSetConstantBuffers1(
		1,
		1,
		m_TerrainDomainShaderConstantBuffer.GetAddressOf(),
		nullptr,
		nullptr
	);
	context->DSSetShader(m_TerrianDomainShader.Get(), nullptr, 0);
	context->DSSetShaderResources(0, 1, m_TerrainShaderResourceView.GetAddressOf());
	context->DSSetSamplers(0, 1, m_CustomMeshSamplerState.GetAddressOf());

	// Draw the objects.
	context->DrawIndexed(
		m_PlaneIndexCount,
		0,
		0
	);
	context->HSSetShader(0, nullptr, 0);
	context->DSSetShader(0, nullptr, 0);
#pragma endregion
#pragma region Draw Clouds
	// Prepare the constant buffer to send it to the graphics device.
	// We just use custom mesh constant buffer because we don't use the model information and the remaining info would be the same
	context->UpdateSubresource1(
		m_CustomMeshConstantBuffer.Get(),
		0,
		NULL,
		&m_CustomMeshConstantBufferData,
		0,
		0,
		0
	);

	// Each vertex is one instance of the VertexPositionColorNormalUV struct.
	stride = sizeof(PositionScalerUV);
	offset = 0;
	context->IASetVertexBuffers(
		0,
		1,
		m_CloudVertexBuffer.GetAddressOf(),
		&stride,
		&offset
	);



	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	context->IASetInputLayout(m_CloudInputLayout.Get());

	// Attach our vertex shader.
	context->VSSetShader(
		m_BillboardVertexShader.Get(),
		nullptr,
		0
	);

	// Send the constant buffer to the graphics device.
	context->VSSetConstantBuffers1(
		0,
		1,
		m_CustomMeshConstantBuffer.GetAddressOf(),
		nullptr,
		nullptr
	);

	// Attach our pixel shader.
	context->PSSetShader(
		m_BillboardPixelShader.Get(),
		nullptr,
		0
	);

	//Using the  custom mesh data for testing, will update textures once this step is confirmed
	context->PSSetShaderResources(0, 1, m_CloudShaderResourceView.GetAddressOf());
	context->PSSetSamplers(0, 1, m_CloudSamplerState.GetAddressOf());
	context->GSSetShader(m_BillboardGeometryShader.Get(), nullptr, 0);
	//Again, we just use the custom mesh's buffers because it already contains the info we need

	context->GSSetConstantBuffers1(
		0,
		1,
		m_CustomMeshConstantBuffer.GetAddressOf(),
		nullptr,
		nullptr
	);
	// Draw the objects.
	context->Draw(m_CloudCount, 0);

	context->GSSetShader(nullptr, nullptr, 0);
#pragma endregion



	CD3D11_BUFFER_DESC LightConstantBufferDesc(sizeof(LightNormalColorPositionType), D3D11_BIND_CONSTANT_BUFFER);
	//Change this dynamically based on current light
	if (currentLight == DIRECTIONAL_LIGHTING) {
		context->UpdateSubresource(LightConstantBuffer.Get(), 0, NULL, &DirectionalLight, 0, 0);
	}
	if (currentLight == POINT_LIGHTING) {
		context->UpdateSubresource(LightConstantBuffer.Get(), 0, NULL, &PointLight, 0, 0);
	}
	if (currentLight == SPOT_LIGHTING) {
		context->UpdateSubresource(LightConstantBuffer.Get(), 0, NULL, &SpotLight, 0, 0);
	}


	context->VSSetConstantBuffers(1, 1, LightConstantBuffer.GetAddressOf());



}

void Sample3DSceneRenderer::CreateDeviceDependentResources()
{
	// Load shaders asynchronously.
	auto loadVSTask = DX::ReadDataAsync(L"InstancedVertexShader.cso");
	auto loadPSTask = DX::ReadDataAsync(L"SamplePixelShader.cso");
	auto loadVSTask2 = DX::ReadDataAsync(L"BillboardVertexShader.cso");
	auto loadPSTask2 = DX::ReadDataAsync(L"BillboardPixelShader.cso");
	auto loadGSTask = DX::ReadDataAsync(L"BillboardGeometryShader.cso");

	auto loadHSTask = DX::ReadDataAsync(L"TerrainHullShader.cso");
	auto loadDSTask = DX::ReadDataAsync(L"TerrainDomainShader.cso");

	// After the vertex shader file is loaded, create the shader and input layout.
	auto createVSTask = loadVSTask.then([this](const std::vector<byte>& fileData) {


		static const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};


#pragma region Pyramid




		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateVertexShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_PyramidvertexShader
			)
		);

		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateInputLayout(
				vertexDesc,
				ARRAYSIZE(vertexDesc),
				&fileData[0],
				fileData.size(),
				&m_PyramidinputLayout
			)
		);

#pragma endregion

#pragma region Custom Mesh
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateVertexShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_CustomMeshVertexShader
			)
		);

		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateInputLayout(
				vertexDesc,
				ARRAYSIZE(vertexDesc),
				&fileData[0],
				fileData.size(),
				&m_CustomMeshInputLayout
			)
		);
#pragma endregion
	});
	//Create Geometry shader
	auto createGSTask = loadGSTask.then([this](const std::vector<byte>& fileData) {





		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateGeometryShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_BillboardGeometryShader
			)
		);



#pragma region Cloud information
		static const PositionScalerUV cloudVerts[] = {
			{XMFLOAT4(0.0f, 3.0f, 0.0f, 1.0f), XMFLOAT2(2.0f, 2.0f), XMFLOAT2(0.0f, 0.0f)}
		};
#pragma endregion


		D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
		vertexBufferData.pSysMem = cloudVerts;
		vertexBufferData.SysMemPitch = 0;
		vertexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(cloudVerts), D3D11_BIND_VERTEX_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&vertexBufferDesc,
				&vertexBufferData,
				&m_CloudVertexBuffer
			)
		);
		m_CloudPoints = cloudVerts[0];

	});
	auto createVSTask2 = loadVSTask2.then([this](const std::vector<byte>& fileData) {


		static const D3D11_INPUT_ELEMENT_DESC geometryBufferDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "SIZE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateVertexShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_BillboardVertexShader
			)
		);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateInputLayout(
				geometryBufferDesc,
				ARRAYSIZE(geometryBufferDesc),
				&fileData[0],
				fileData.size(),
				&m_CloudInputLayout
			)
		);

#pragma region Cube



		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateVertexShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_PlaneVertexShader
			)
		);



		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateInputLayout(
				geometryBufferDesc,
				ARRAYSIZE(geometryBufferDesc),
				&fileData[0],
				fileData.size(),
				&m_PlaneInputLayout
			)
		);
#pragma endregion


	});
	auto createPSTask2 = loadPSTask2.then([this](const std::vector<byte>& fileData) {

		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreatePixelShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_BillboardPixelShader
			)
		);



		D3D11_SAMPLER_DESC samplerDesc;
		ZeroMemory(&samplerDesc, sizeof(samplerDesc));
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.MaxAnisotropy = 0;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		samplerDesc.BorderColor[0] = 0.0f;
		samplerDesc.BorderColor[1] = 0.0f;
		samplerDesc.BorderColor[2] = 0.0f;
		samplerDesc.BorderColor[3] = 0.0f;

		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateSamplerState(
				&samplerDesc,
				&m_CloudSamplerState
			)
		);

		CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), L"Assets/Cloud1.dds", nullptr, &m_CloudShaderResourceView);
		CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), L"Assets/Terrain map.dds", nullptr, &m_TerrainShaderResourceView);

	});
	// After the pixel shader file is loaded, create the shader and constant buffer.
	auto createPSTask = loadPSTask.then([this](const std::vector<byte>& fileData) {

		CD3D11_BUFFER_DESC LightConstantBufferDesc(sizeof(LightNormalColorPositionType), D3D11_BIND_CONSTANT_BUFFER);

		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateBuffer(
			&LightConstantBufferDesc,
			nullptr,
			&LightConstantBuffer)
		);


		CD3D11_BUFFER_DESC constantBufferDesc(sizeof(ModelViewProjectionConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
		D3D11_SAMPLER_DESC samplerDesc;
		ZeroMemory(&samplerDesc, sizeof(samplerDesc));
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.MaxAnisotropy = 0;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		samplerDesc.BorderColor[0] = 0.0f;
		samplerDesc.BorderColor[1] = 0.0f;
		samplerDesc.BorderColor[2] = 0.0f;
		samplerDesc.BorderColor[3] = 0.0f;

		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateSamplerState(
				&samplerDesc,
				&m_CustomMeshSamplerState
			)
		);

#pragma region Cube
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreatePixelShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_PlanePixelShader
			)
		);


		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&constantBufferDesc,
				nullptr,
				&m_PlaneConstantBuffer
			)
		);
#pragma endregion

#pragma region Pyramid
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreatePixelShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_PyramidpixelShader
			)
		);


		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&constantBufferDesc,
				nullptr,
				&m_PyramidconstantBuffer
			)
		);
#pragma endregion
#pragma region Custom Mesh
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreatePixelShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_CustomMeshPixelShader
			)
		);


		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&constantBufferDesc,
				nullptr,
				&m_CustomMeshConstantBuffer
			)
		);

		CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), L"Assets/rock.dds", nullptr, &m_CustomMeshShaderResourceView);
#pragma endregion
	});
	auto createHSTask = loadHSTask.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateHullShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_TerrianHullShader
			)
		);
		CD3D11_BUFFER_DESC hullShaderBufferDesc(sizeof(HullShaderData), D3D11_BIND_CONSTANT_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&hullShaderBufferDesc,
				nullptr,
				&m_TerrainHullShaderConstantBuffer
			)
		);
	});
	auto createDSTask = loadDSTask.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateDomainShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_TerrianDomainShader
			)
		);

		CD3D11_BUFFER_DESC DomainShaderBufferDesc(sizeof(DomainShaderData), D3D11_BIND_CONSTANT_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&DomainShaderBufferDesc,
				nullptr,
				&m_TerrainDomainShaderConstantBuffer
			)
		);
	});


	// Once both shaders are loaded, create the mesh.
	auto createCubeTask = (createPSTask && createVSTask && createGSTask && createPSTask2 && createVSTask2 && createHSTask && createDSTask).then([this]() {
#pragma region Create cube verts



		// Load mesh vertices. Each vertex has a position and a color.
		static const PositionScalerUV cubeVertices[] =
		{

			{ XMFLOAT4(-20.0f, -10.0f,  -20.0f, 1.0f),	XMFLOAT2(1.0f, 1.0f), XMFLOAT2(0.1f, 0.1f) },
			{ XMFLOAT4(-20.0f, -10.0f,  20.0f, 1.0f),	XMFLOAT2(1.0f, 1.0f), XMFLOAT2(0.1f, 1.0f) },
			{ XMFLOAT4(20.0f,  -10.0f,  -20.0f, 1.0f),	XMFLOAT2(1.0f, 1.0f), XMFLOAT2(1.0f, 0.1f) },
			{ XMFLOAT4(20.0f,  -10.0f,  20.0f, 1.0f),	XMFLOAT2(1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
		};



		D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
		vertexBufferData.pSysMem = cubeVertices;
		vertexBufferData.SysMemPitch = 0;
		vertexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(cubeVertices), D3D11_BIND_VERTEX_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&vertexBufferDesc,
				&vertexBufferData,
				&m_PlaneVertexBuffer
			)
		);

		// Load mesh indices. Each trio of indices represents
		// a triangle to be rendered on the screen.
		// For example: 0,2,1 means that the vertices with indexes
		// 0, 2 and 1 from the vertex buffer compose the 
		// first triangle of this mesh.
		static const unsigned short cubeIndices[] =
		{


			2,1,0,
			3,1,2
		};



		m_PlaneIndexCount = ARRAYSIZE(cubeIndices);

		D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
		indexBufferData.pSysMem = cubeIndices;
		indexBufferData.SysMemPitch = 0;
		indexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC indexBufferDesc(sizeof(cubeIndices), D3D11_BIND_INDEX_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&indexBufferDesc,
				&indexBufferData,
				&m_PlaneIndexBuffer
			)
		);
#pragma endregion

#pragma region Create Pyramid Verts







		static const VertexPositionColorNormalUV pyramidVerts[] =
		{
			{ XMFLOAT3(-0.5f, 0.5f, -0.5f), XMFLOAT3(1.0f, 1.0f, 0.5f) },
			{ XMFLOAT3(0.5f, 0.5f, -0.5f), XMFLOAT3(1.0f, 1.0f, 0.5f) },
			{ XMFLOAT3(0.5f, 0.5f, 0.5f), XMFLOAT3(1.0f, 1.0f, 0.5f) },
			{ XMFLOAT3(-0.5f, 0.5f, 0.5f), XMFLOAT3(1.0f, 1.0f, 0.5f) },
			{ XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },


		};

		vertexBufferData = { 0 };
		vertexBufferData.pSysMem = pyramidVerts;
		vertexBufferData.SysMemPitch = 0;
		vertexBufferData.SysMemSlicePitch = 0;
		vertexBufferDesc = CD3D11_BUFFER_DESC(sizeof(pyramidVerts), D3D11_BIND_VERTEX_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&vertexBufferDesc,
				&vertexBufferData,
				&m_PyramidvertexBuffer
			)
		);

		static const unsigned short pyramidIndices[] =
		{
			//Bottom face
			2,1,0,
			0,3,2,

			//+z
			3,4,2,

			//-z
			1,4,0,

			//+x
			2,4,1,

			//-x
			0,4,3,

		};




		m_PyramidindexCount = ARRAYSIZE(pyramidIndices);

		indexBufferData = { 0 };
		indexBufferData.pSysMem = pyramidIndices;
		indexBufferData.SysMemPitch = 0;
		indexBufferData.SysMemSlicePitch = 0;
		indexBufferDesc = CD3D11_BUFFER_DESC(sizeof(pyramidIndices), D3D11_BIND_INDEX_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&indexBufferDesc,
				&indexBufferData,
				&m_PyramidindexBuffer
			)
		);
#pragma endregion


		CreateCustomMesh();
	});

	// Once the cube is loaded, the object is ready to be rendered.
	createCubeTask.then([this]() {
		m_loadingComplete = true;
	});
}

void Sample3DSceneRenderer::CreateCustomMesh()
{
	Mesh testMesh;
	testMesh.LoadMeshFromFile("Assets/asteroid.obj");
#pragma region Create Mesh Verts 1
	std::vector<VertexPositionColorNormalUV> MeshVerts;

	std::vector<short> MeshIndexes;
	for (unsigned int i = 0; i < testMesh.UniqueVertexArray.size(); i++)
	{
		MeshVerts.push_back(
		{ XMFLOAT3(testMesh.UniqueVertexArray[i].m_position),
		  XMFLOAT3(
				0.0f,
				1.0f,
				0.0f),
		  XMFLOAT3(testMesh.UniqueVertexArray[i].m_normalVec),
		XMFLOAT2(testMesh.UniqueVertexArray[i].m_UVcoords.x, testMesh.UniqueVertexArray[i].m_UVcoords.y) });

#ifdef _TRANSLATEMODELS
		MeshVerts[MeshVerts.size() - 1].pos.x += 0.5f;
#endif
	}
	MeshVerts.shrink_to_fit();
	for (unsigned int i = 0; i < testMesh.TrianglePointIndexes.size(); i++)
	{
		MeshIndexes.push_back(testMesh.TrianglePointIndexes[i]);
	}
	MeshIndexes.shrink_to_fit();
#pragma endregion
#pragma region Create Mesh Verts 2

	D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
	vertexBufferData.pSysMem = MeshVerts.data();
	vertexBufferData.SysMemPitch = 0;
	vertexBufferData.SysMemSlicePitch = 0;
	CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(VertexPositionColorNormalUV) * MeshVerts.size(), D3D11_BIND_VERTEX_BUFFER);
	DX::ThrowIfFailed(
		m_deviceResources->GetD3DDevice()->CreateBuffer(
			&vertexBufferDesc,
			&vertexBufferData,
			&m_CustomMeshVertexBuffer
		)
	);

	m_CustomMeshIndexCount = MeshIndexes.size();

	D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
	indexBufferData.pSysMem = MeshIndexes.data();
	indexBufferData.SysMemPitch = 0;
	indexBufferData.SysMemSlicePitch = 0;
	CD3D11_BUFFER_DESC indexBufferDesc(sizeof(short) * MeshIndexes.size(), D3D11_BIND_INDEX_BUFFER);
	DX::ThrowIfFailed(
		m_deviceResources->GetD3DDevice()->CreateBuffer(
			&indexBufferDesc,
			&indexBufferData,
			&m_CustomMeshIndexBuffer
		)
	);
#pragma endregion
}

void Sample3DSceneRenderer::ReleaseDeviceDependentResources()
{
	m_loadingComplete = false;
	m_PlaneVertexShader.Reset();
	m_PlaneInputLayout.Reset();
	m_PlanePixelShader.Reset();
	m_PlaneConstantBuffer.Reset();
	m_PlaneVertexBuffer.Reset();
	m_PlaneIndexBuffer.Reset();

	m_PyramidinputLayout.Reset();
	m_PyramidvertexBuffer.Reset();
	m_PyramidindexBuffer.Reset();
	m_PyramidvertexShader.Reset();
	m_PyramidpixelShader.Reset();
	m_PyramidconstantBuffer.Reset();

	m_CustomMeshInputLayout.Reset();
	m_CustomMeshVertexBuffer.Reset();
	m_CustomMeshIndexBuffer.Reset();
	m_CustomMeshVertexShader.Reset();
	m_CustomMeshPixelShader.Reset();
	m_CustomMeshConstantBuffer.Reset();
	//m_CustomMeshResource.Reset();
	m_CustomMeshShaderResourceView.Reset();

	LightConstantBuffer.Reset();
}