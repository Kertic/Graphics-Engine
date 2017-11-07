#include "pch.h"
#include "Sample3DSceneRenderer.h"
#include "Mesh.h"

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
	currentLight = POINT_LIGHTING;
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
	static const XMVECTORF32 eye = { 0.0f, 0.0f, -1.5f, 0.0f };
	static const XMVECTORF32 at = { 0.0f, 0.0f, 0.0f, 0.0f };
	static const XMVECTORF32 up = { 0.0f, 1.0f, 0.0f, 0.0f };
	XMStoreFloat4x4(&camera, XMMatrixLookAtRH(eye, at, up));
	XMStoreFloat4x4(&m_PlaneConstantBufferData.view, XMMatrixTranspose(XMMatrixInverse(0, XMMatrixLookAtRH(eye, at, up))));
	XMStoreFloat4x4(&m_CustomMeshConstantBufferData.view, XMMatrixTranspose(XMMatrixInverse(0, XMMatrixLookAtRH(eye, at, up))));
	XMStoreFloat4x4(&m_PyramidconstantBufferData.view, XMMatrixTranspose(XMMatrixInverse(0, XMMatrixLookAtRH(eye, at, up))));


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

	XMMATRIX newcamera = XMLoadFloat4x4(&camera);

	if (buttons['W'])
	{
		newcamera.r[3] = newcamera.r[3] + newcamera.r[2] * -timer.GetElapsedSeconds() * 5.0;
	}

	if (a_down)
	{
		newcamera.r[3] = newcamera.r[3] + newcamera.r[0] * -timer.GetElapsedSeconds() *5.0;
	}

	if (s_down)
	{
		newcamera.r[3] = newcamera.r[3] + newcamera.r[2] * timer.GetElapsedSeconds() * 5.0;
	}

	if (d_down)
	{
		newcamera.r[3] = newcamera.r[3] + newcamera.r[0] * timer.GetElapsedSeconds() * 5.0;
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

	Windows::UI::Input::PointerPoint^ point = nullptr;

	//if(mouse_move)/*This crashes unless a mouse event actually happened*/
		//point = Windows::UI::Input::PointerPoint::GetCurrentPoint(pointerID);

	if (mouse_move)
	{
		// Updates the application state once per frame.
		if (left_click)
		{
			XMVECTOR pos = newcamera.r[3];
			newcamera.r[3] = XMLoadFloat4(&XMFLOAT4(0, 0, 0, 1));
			newcamera = XMMatrixRotationX(-diffy*0.01f) * newcamera * XMMatrixRotationY(-diffx*0.01f);
			newcamera.r[3] = pos;
		}
	}

#pragma region Update Lights
	DirectionalLight = {
		XMFLOAT4(0.0f, -0.5f + light_DynamicOffset, 0.5f + light_DynamicOffset, 1.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		XMFLOAT4((float)CurrentLightingOptions::DIRECTIONAL_LIGHTING, 0.0f, 0.0f, 0.0f) };

	PointLight = {
		XMFLOAT4(0.0f, -0.5f, 0.5f, 1.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		XMFLOAT4(0.0f, -1.0f + light_DynamicOffset, 0.0f, 1.0f),
		XMFLOAT4((float)CurrentLightingOptions::POINT_LIGHTING, 0.0f, 0.0f, 0.0f) };

	SpotLight = {
		XMFLOAT4(0.0f, -0.5f , 0.0f + light_DynamicOffset, 1.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		XMFLOAT4(0.0f, 1.0f + light_DynamicOffset, 0.0f, 1.0f),
		XMFLOAT4((float)CurrentLightingOptions::SPOT_LIGHTING, 0.0f, 0.0f, 0.0f) };
#pragma endregion


	XMStoreFloat4x4(&camera, newcamera);

	/*Be sure to inverse the camera & Transpose because they don't use pragma pack row major in shaders*/
	XMStoreFloat4x4(&m_PlaneConstantBufferData.view, XMMatrixTranspose(XMMatrixInverse(0, newcamera)));
	XMStoreFloat4x4(&m_PyramidconstantBufferData.view, XMMatrixTranspose(XMMatrixInverse(0, newcamera)));
	XMStoreFloat4x4(&m_CustomMeshConstantBufferData.view, XMMatrixTranspose(XMMatrixInverse(0, newcamera)));

	mouse_move = false;/*Reset*/
}

// Rotate the 3D cube model a set amount of radians.
void Sample3DSceneRenderer::Rotate(float radians)
{
	// Prepare to pass the updated model matrix to the shader
	XMStoreFloat4x4(&m_PlaneConstantBufferData.model[0], XMMatrixTranspose(XMMatrixRotationY(radians)));

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

	// Each vertex is one instance of the VertexPositionColorNormal struct.
	UINT stride = sizeof(VertexPositionColorNormal);
	UINT offset = 0;
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

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->IASetInputLayout(m_PlaneInputLayout.Get());

	// Attach our vertex shader.
	context->VSSetShader(
		m_PlaneVertexShader.Get(),
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
		m_PlanePixelShader.Get(),
		nullptr,
		0
	);

	// Draw the objects.
	context->DrawIndexed(
		m_PlaneIndexCount,
		0,
		0
	);

#pragma endregion
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
	//	// Each vertex is one instance of the VertexPositionColorNormal struct.
	//	stride = sizeof(VertexPositionColorNormal);
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

	// Each vertex is one instance of the VertexPositionColorNormal struct.
	stride = sizeof(VertexPositionColorNormal);
	offset = 0;
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

	// Draw the objects.
	context->DrawIndexedInstanced(m_CustomMeshIndexCount, MAX_INSTANCES_OF_GEOMETRY, 0, 0, 0);

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

	// After the vertex shader file is loaded, create the shader and input layout.
	auto createVSTask = loadVSTask.then([this](const std::vector<byte>& fileData) {


		static const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
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
				vertexDesc,
				ARRAYSIZE(vertexDesc),
				&fileData[0],
				fileData.size(),
				&m_PlaneInputLayout
			)
		);
#pragma endregion

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

	// After the pixel shader file is loaded, create the shader and constant buffer.
	auto createPSTask = loadPSTask.then([this](const std::vector<byte>& fileData) {

		CD3D11_BUFFER_DESC LightConstantBufferDesc(sizeof(LightNormalColorPositionType), D3D11_BIND_CONSTANT_BUFFER);

		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateBuffer(
			&LightConstantBufferDesc,
			nullptr,
			&LightConstantBuffer)
		);


		CD3D11_BUFFER_DESC constantBufferDesc(sizeof(ModelViewProjectionConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
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
#pragma endregion
	});

	// Once both shaders are loaded, create the mesh.
	auto createCubeTask = (createPSTask && createVSTask).then([this]() {
#pragma region Create cube verts



		// Load mesh vertices. Each vertex has a position and a color.
		static const VertexPositionColorNormal cubeVertices[] =
		{

			{ XMFLOAT3(-5.0f, -2.0f,  -5.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
			{ XMFLOAT3(-5.0f, -2.0f,  5.0f), XMFLOAT3(0.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
			{ XMFLOAT3(5.0f,  -2.0f,  -5.0f), XMFLOAT3(1.0f, 0.0f, 1.0f) , XMFLOAT3(0.0f, 1.0f, 0.0f) },
			{ XMFLOAT3(5.0f,  -2.0f,  5.0f), XMFLOAT3(1.0f, 1.0f, 1.0f) , XMFLOAT3(0.0f, 1.0f, 0.0f) },
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







		static const VertexPositionColorNormal pyramidVerts[] =
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
	testMesh.LoadMeshFromFile("Assets/test pyramid.obj");
#pragma region Create Mesh Verts 1
	std::vector<VertexPositionColorNormal> MeshVerts;

	std::vector<short> MeshIndexes;
	for (unsigned int i = 0; i < testMesh.UniqueVertexArray.size(); i++)
	{
		MeshVerts.push_back(
		{ XMFLOAT3(testMesh.UniqueVertexArray[i].m_position),
		  XMFLOAT3(
				0.0f,
				1.0f,
				0.0f),
		  XMFLOAT3(testMesh.UniqueVertexArray[i].m_normalVec) });

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
	CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(VertexPositionColorNormal) * MeshVerts.size(), D3D11_BIND_VERTEX_BUFFER);
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

	LightConstantBuffer.Reset();
}