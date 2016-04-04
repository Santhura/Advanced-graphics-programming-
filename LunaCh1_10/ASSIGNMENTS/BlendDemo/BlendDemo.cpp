//***************************************************************************************
// BoxDemo.cpp by Frank Luna (C) 2011 All Rights Reserved.
//
// Demonstrates rendering a colored box.
//
// Controls:
//		Hold the left mouse button down and move the mouse to rotate.
//      Hold the right mouse button down to zoom in and out.
//
//***************************************************************************************

#include "d3dApp.h"
#include "d3dx11Effect.h"
#include "MathHelper.h"
#include "RenderStates.h"
#include "Effects.h"
struct Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
	std::vector<Vertex> Vertices;
};

class BlendDemo : public D3DApp
{
public:
	BlendDemo(HINSTANCE hInstance);
	~BlendDemo();

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene();

	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);

private:
	void BuildGeometryBuffers();
	void BuildGeometryBuffers2();

	void BuildFX();
	void BuildVertexLayout();

private:
	ID3D11Buffer* mBoxVB;
	ID3D11Buffer* mBoxIB;
	ID3D11Buffer* mBoxVB2;
	ID3D11Buffer* mBoxIB2;
	ID3D11Buffer* mBoxVB3;
	ID3D11Buffer* mBoxIB3;

	ID3DX11Effect* mFX;
	ID3DX11EffectTechnique* mTech;
	ID3DX11EffectTechnique* mTech2;
	ID3DX11EffectTechnique* mTech3;

	ID3DX11EffectMatrixVariable* mfxWorldViewProj;

	ID3D11InputLayout* mInputLayout;

	UINT mTriangle1;
	UINT mTriangle2;

	XMFLOAT4X4 mWorld;
	XMFLOAT4X4 mView;
	XMFLOAT4X4 mProj;

	float mTheta;
	float mPhi;
	float mRadius;

	POINT mLastMousePos;
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	BlendDemo theApp(hInstance);

	if (!theApp.Init())
		return 0;

	return theApp.Run();
}


BlendDemo::BlendDemo(HINSTANCE hInstance)
	: D3DApp(hInstance), mBoxVB(0), mBoxIB(0), mFX(0), mTech(0), mTech2(0), mBoxVB2(0), mBoxIB2(0), mBoxIB3(0), mBoxVB3(0), mTech3(0),
	mfxWorldViewProj(0), mInputLayout(0),
	mTheta(1.5f*MathHelper::Pi), mPhi(0.25f*MathHelper::Pi), mRadius(5.0f)
{
	mMainWndCaption = L"Blend Demo";

	mLastMousePos.x = 0;
	mLastMousePos.y = 0;

	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&mWorld, I);
	XMStoreFloat4x4(&mView, I);
	XMStoreFloat4x4(&mProj, I);
}

BlendDemo::~BlendDemo()
{
	ReleaseCOM(mBoxVB);
	ReleaseCOM(mBoxIB);
	ReleaseCOM(mBoxVB2);
	ReleaseCOM(mBoxIB2);
	ReleaseCOM(mBoxVB3);
	ReleaseCOM(mBoxIB3);
	ReleaseCOM(mFX);
	ReleaseCOM(mInputLayout);
}

bool BlendDemo::Init()
{
	if (!D3DApp::Init())
		return false;

	RenderStates::InitAll(md3dDevice);
	Effects::InitAll(md3dDevice);
	BuildGeometryBuffers();
	BuildGeometryBuffers2();
	BuildFX();
	BuildVertexLayout();
	

	return true;
}

void BlendDemo::OnResize()
{
	D3DApp::OnResize();

	// The window resized, so update the aspect ratio and recompute the projection matrix.
	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f*MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&mProj, P);
}

void BlendDemo::UpdateScene(float dt)
{
	// Convert Spherical to Cartesian coordinates.
	float x = mRadius*sinf(mPhi)*cosf(mTheta);
	float z = mRadius*sinf(mPhi)*sinf(mTheta);
	float y = mRadius*cosf(mPhi);

	// Build the view matrix.
	XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX V = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&mView, V);
}

void BlendDemo::DrawScene()
{
	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::LightSteelBlue));
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	md3dImmediateContext->IASetInputLayout(mInputLayout);
	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	

	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	// Set constants
	XMMATRIX world = XMLoadFloat4x4(&mWorld);
	XMMATRIX view = XMLoadFloat4x4(&mView);
	XMMATRIX proj = XMLoadFloat4x4(&mProj);
	XMMATRIX worldViewProj = world*view*proj;

	mfxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&worldViewProj));

	D3DX11_TECHNIQUE_DESC techDesc;


	mTech2->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{

		md3dImmediateContext->IASetVertexBuffers(0, 1, &mBoxVB2, &stride, &offset);
		md3dImmediateContext->IASetIndexBuffer(mBoxIB2, DXGI_FORMAT_R32_UINT, 0);
		ID3DX11EffectPass* pass = mTech2->GetPassByIndex(p);

		mTech2->GetPassByIndex(p)->Apply(0, md3dImmediateContext);

		md3dImmediateContext->OMSetBlendState(RenderStates::AlphaBlending, blendFactor, 0xffffffff);

	//	md3dImmediateContext->OMSetBlendState(RenderStates::MultiplicativeBlending, blendFactor, 0xffffffff);
	//	md3dImmediateContext->OMSetBlendState(RenderStates::AdditiveBlending, blendFactor, 0xffffffff);
		//md3dImmediateContext->OMSetBlendState(RenderStates::MultiplicativeBlendingX2, blendFactor, 0xffffffff);

		pass->Apply(0, md3dImmediateContext);
		// 36 indices for the box.
		md3dImmediateContext->DrawIndexed(6, 0, 0);
		md3dImmediateContext->DrawIndexed(6, 3, 3);

	}
	mTech3->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{

		md3dImmediateContext->IASetVertexBuffers(0, 1, &mBoxVB3, &stride, &offset);
		md3dImmediateContext->IASetIndexBuffer(mBoxIB3, DXGI_FORMAT_R32_UINT, 0);
		
		ID3DX11EffectPass* pass = mTech3->GetPassByIndex(p);

		mTech3->GetPassByIndex(p)->Apply(0, md3dImmediateContext);

		//	md3dImmediateContext->OMSetBlendState(RenderStates::AlphaBlending, blendFactor, 0xffffffff);

		//md3dImmediateContext->OMSetBlendState(RenderStates::MultiplicativeBlending, blendFactor, 0xffffffff);
			md3dImmediateContext->OMSetBlendState(RenderStates::AdditiveBlending, blendFactor, 0xffffffff);
	//	md3dImmediateContext->OMSetBlendState(RenderStates::MultiplicativeBlendingX2, blendFactor, 0xffffffff);

		pass->Apply(0, md3dImmediateContext);
		// 36 indices for the box.
		md3dImmediateContext->DrawIndexed(6, 0, 0);
		md3dImmediateContext->DrawIndexed(6, 3, 3);

	}
	mTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		md3dImmediateContext->IASetVertexBuffers(0, 1, &mBoxVB, &stride, &offset);
		md3dImmediateContext->IASetIndexBuffer(mBoxIB, DXGI_FORMAT_R32_UINT, 0);
		ID3DX11EffectPass* pass = mTech->GetPassByIndex(p);

		mTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);

		//md3dImmediateContext->OMSetBlendState(RenderStates::AlphaBlending, blendFactor, 0xffffffff);
		//md3dImmediateContext->OMSetBlendState(RenderStates::AdditiveBlending, blendFactor, 0xffffffff);
		md3dImmediateContext->OMSetBlendState(RenderStates::MultiplicativeBlending, blendFactor, 0xffffffff);
		//md3dImmediateContext->OMSetBlendState(RenderStates::MultiplicativeBlendingX2, blendFactor, 0xffffffff);

		pass->Apply(0, md3dImmediateContext);
		// 36 indices for the box.
		md3dImmediateContext->DrawIndexed(6, 0, 0);
		md3dImmediateContext->DrawIndexed(6, 3, 3);

	}

	HR(mSwapChain->Present(0, 0));
}

void BlendDemo::OnMouseDown(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	SetCapture(mhMainWnd);
}

void BlendDemo::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void BlendDemo::OnMouseMove(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_LBUTTON) != 0)
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f*static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(y - mLastMousePos.y));

		// Update angles based on input to orbit camera around box.
		mTheta += dx;
		mPhi += dy;

		// Restrict the angle mPhi.
		mPhi = MathHelper::Clamp(mPhi, 0.1f, MathHelper::Pi - 0.1f);
	}
	else if ((btnState & MK_RBUTTON) != 0)
	{
		// Make each pixel correspond to 0.005 unit in the scene.
		float dx = 0.005f*static_cast<float>(x - mLastMousePos.x);
		float dy = 0.005f*static_cast<float>(y - mLastMousePos.y);

		// Update the camera radius based on input.
		mRadius += dx - dy;

		// Restrict the radius.
		mRadius = MathHelper::Clamp(mRadius, 3.0f, 15.0f);
	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}

void BlendDemo::BuildGeometryBuffers()
{
	//first group

	// Create vertex buffer
	Vertex triangle1[] =
	{
		{ XMFLOAT3(-1.0f, -1.0f, +1.0f), (const float*)&Colors::RedHalf },
		{ XMFLOAT3(-1.0f, +1.0f, +1.0f), (const float*)&Colors::RedHalf },
		{ XMFLOAT3(+1.0f, +1.0f, +1.0f), (const float*)&Colors::RedHalf },

		{ XMFLOAT3(-1.0f, -1.0f, 0.0f), (const float*)&Colors::BlueHalf },
		{ XMFLOAT3(-1.0f, +1.0f, 0.0f), (const float*)&Colors::BlueHalf },
		{ XMFLOAT3(+1.0f, +1.0f, 0.0f), (const float*)&Colors::BlueHalf }

	
	};

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex) * 6;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = triangle1;
	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mBoxVB));


	// Create the index buffer

	UINT indices1[] = {
		0, 1, 2,
		3, 4, 5
	};

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * 6;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = indices1;
	HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mBoxIB));

	// second group

	// Create vertex buffer
	Vertex triangle2[] =
	{

		{ XMFLOAT3(-2.0f, -1.0f, .5f), (const float*)&Colors::RedHalf },
		{ XMFLOAT3(-2.0f, +1.0f, .5f), (const float*)&Colors::RedHalf },
		{ XMFLOAT3(-1.0f, +1.0f, .5f), (const float*)&Colors::RedHalf },

		{ XMFLOAT3(-2.0f, -1.0f,-1.5f), (const float*)&Colors::BlueHalf },
		{ XMFLOAT3(-2.0f, +1.0f, -1.5f), (const float*)&Colors::BlueHalf },
		{ XMFLOAT3(-1.0f, +1.0f, -1.5f), (const float*)&Colors::BlueHalf }
	};

	D3D11_BUFFER_DESC vbd2;
	vbd2.Usage = D3D11_USAGE_IMMUTABLE;
	vbd2.ByteWidth = sizeof(Vertex) * 6;
	vbd2.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd2.CPUAccessFlags = 0;
	vbd2.MiscFlags = 0;
	vbd2.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA vinitData2;
	vinitData2.pSysMem = triangle2;
	HR(md3dDevice->CreateBuffer(&vbd2, &vinitData2, &mBoxVB2));


	// Create the index buffer

	UINT indices2[] = {
		0, 1, 2,
		3, 4, 5
	};

	D3D11_BUFFER_DESC ibd2;
	ibd2.Usage = D3D11_USAGE_IMMUTABLE;
	ibd2.ByteWidth = sizeof(UINT) * 6;
	ibd2.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd2.CPUAccessFlags = 0;
	ibd2.MiscFlags = 0;
	ibd2.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA iinitData2;
	iinitData2.pSysMem = indices2;
	HR(md3dDevice->CreateBuffer(&ibd2, &iinitData2, &mBoxIB2));


	// thrird group

	// Create vertex buffer
	Vertex triangle3[] =
	{

		{ XMFLOAT3(1.0f, -1.0f, 3.0f), (const float*)&Colors::RedHalf },
		{ XMFLOAT3(1.0f, +1.0f, 3.0f), (const float*)&Colors::RedHalf },
		{ XMFLOAT3(+2.0f, +1.0f, 3.0f), (const float*)&Colors::RedHalf },

		{ XMFLOAT3(1.0f, -1.0f, 2.0f), (const float*)&Colors::BlueHalf },
		{ XMFLOAT3(1.0f, +1.0f, 2.0f), (const float*)&Colors::BlueHalf },
		{ XMFLOAT3(+2.0f, +1.0f, 2.0f), (const float*)&Colors::BlueHalf }
	};

	D3D11_BUFFER_DESC vbd3;
	vbd3.Usage = D3D11_USAGE_IMMUTABLE;
	vbd3.ByteWidth = sizeof(Vertex) * 6;
	vbd3.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd3.CPUAccessFlags = 0;
	vbd3.MiscFlags = 0;
	vbd3.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA vinitData3;
	vinitData3.pSysMem = triangle3;
	HR(md3dDevice->CreateBuffer(&vbd3, &vinitData3, &mBoxVB3));


	// Create the index buffer

	UINT indices3[] = {
		0, 1, 2,
		3, 4, 5
	};

	D3D11_BUFFER_DESC ibd3;
	ibd3.Usage = D3D11_USAGE_IMMUTABLE;
	ibd3.ByteWidth = sizeof(UINT) * 6;
	ibd3.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd3.CPUAccessFlags = 0;
	ibd3.MiscFlags = 0;
	ibd3.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA iinitData3;
	iinitData3.pSysMem = indices3;
	HR(md3dDevice->CreateBuffer(&ibd3, &iinitData3, &mBoxIB3));
}

void BlendDemo::BuildGeometryBuffers2()
{

}

void BlendDemo::BuildFX()
{
	DWORD shaderFlags = 0;
#if defined( DEBUG ) || defined( _DEBUG )
	shaderFlags |= D3D10_SHADER_DEBUG;
	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

	ID3D10Blob* compiledShader = 0;
	ID3D10Blob* compilationMsgs = 0;
	HRESULT hr = D3DX11CompileFromFile(L"FX/color.fx", 0, 0, 0, "fx_5_0", shaderFlags,
		0, 0, &compiledShader, &compilationMsgs, 0);

	// compilationMsgs can store errors or warnings.
	if (compilationMsgs != 0)
	{
		MessageBoxA(0, (char*)compilationMsgs->GetBufferPointer(), 0, 0);
		ReleaseCOM(compilationMsgs);
	}

	// Even if there are no compilationMsgs, check to make sure there were no other errors.
	if (FAILED(hr))
	{
		DXTrace(__FILE__, (DWORD)__LINE__, hr, L"D3DX11CompileFromFile", true);
	}

	HR(D3DX11CreateEffectFromMemory(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(),
		0, md3dDevice, &mFX));

	// Done with compiled shader.
	ReleaseCOM(compiledShader);

	mTech = mFX->GetTechniqueByName("ColorTech");
	mTech2 = mFX->GetTechniqueByName("ColorTech");
	mTech3 = mFX->GetTechniqueByName("ColorTech");

	mfxWorldViewProj = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
}

void BlendDemo::BuildVertexLayout()
{
	// Create the vertex input layout.
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	// Create the input layout
	D3DX11_PASS_DESC passDesc;
	mTech->GetPassByIndex(0)->GetDesc(&passDesc);
	mTech2->GetPassByIndex(0)->GetDesc(&passDesc);
	mTech3->GetPassByIndex(0)->GetDesc(&passDesc);


	HR(md3dDevice->CreateInputLayout(vertexDesc, 2, passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize, &mInputLayout));
}
