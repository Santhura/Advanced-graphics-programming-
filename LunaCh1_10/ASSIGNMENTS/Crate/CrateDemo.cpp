//***************************************************************************************
// CrateDemo.cpp by Frank Luna (C) 2011 All Rights Reserved.
//
// Demonstrates texturing a box.
//
// Controls:
//		Hold the left mouse button down and move the mouse to rotate.
//      Hold the right mouse button down to zoom in and out.
//
//***************************************************************************************

#include "d3dApp.h"
#include "d3dx11Effect.h"
#include "GeometryGenerator.h"
#include "MathHelper.h"
#include "LightHelper.h"
#include "Effects.h"
#include "Vertex.h"

class CrateApp : public D3DApp
{
public:
	CrateApp(HINSTANCE hInstance);
	~CrateApp();

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene(); 

	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);

private:
	void BuildGeometryBuffers();
	void BuildPhoneGeometryBuffers();

private:
	ID3D11Buffer* mBoxVB;
	ID3D11Buffer* mPhoneCaseVB; //edit
	ID3D11Buffer* mBoxIB;
	ID3D11Buffer* mPhoneCaseIB; // edit
	ID3D11Buffer* mPhoneScreenIB; // edit
	ID3D11Buffer* mPhoneScreenVB; // edit


	ID3D11ShaderResourceView* mDiffuseMapSRV;
	ID3D11ShaderResourceView* mPhoneDiffuseMApSRV; // edit
	ID3D11ShaderResourceView* mScreenDiffuseMapSRV; //edit

	DirectionalLight mDirLights[3];
	Material mBoxMat;
	Material mPhoneCaseMat; //edit

	XMFLOAT4X4 mTexTransform;
	XMFLOAT4X4 mBoxWorld;
	XMFLOAT4X4 mPhoneCaseWorld; //edit

	XMFLOAT4X4 mView;
	XMFLOAT4X4 mProj;

	int mBoxVertexOffset;
	int mPhoneCaseVertexOffset; // edit
	UINT mBoxIndexOffset;
	UINT mPhoneCaseIndexOffset; // edit
	UINT mBoxIndexCount;
	UINT mPhoneCaseIndexCount; // edit

	int mphoneScreenVertexOffset;
	UINT mPhoneScreenIndexOffset;
	UINT mPhoneScreenIndexCount;

	ID3D11RasterizerState* mWireframeRS;	//edit: object wireframe

	XMFLOAT3 mEyePosW;

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
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	CrateApp theApp(hInstance);
	
	if( !theApp.Init() )
		return 0;
	
	return theApp.Run();
}
 

CrateApp::CrateApp(HINSTANCE hInstance)
: D3DApp(hInstance), mBoxVB(0), mBoxIB(0), mPhoneCaseIB(0), mPhoneCaseVB(0), mPhoneScreenIB(0), mPhoneScreenVB(0), mDiffuseMapSRV(0), mEyePosW(0.0f, 0.0f, 0.0f), 
  mTheta(1.3f*MathHelper::Pi), mPhi(0.4f*MathHelper::Pi), mRadius(2.5f)
{
	mMainWndCaption = L"Crate Demo";
	
	mLastMousePos.x = 0;
	mLastMousePos.y = 0;

	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&mBoxWorld, I);
	XMStoreFloat4x4(&mTexTransform, I);
	XMStoreFloat4x4(&mView, I);
	XMStoreFloat4x4(&mProj, I);
	
	XMMATRIX J = XMMatrixIdentity();
	XMStoreFloat4x4(&mPhoneCaseWorld, J);

	mDirLights[0].Ambient  = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	mDirLights[0].Diffuse  = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	mDirLights[0].Specular = XMFLOAT4(0.6f, 0.6f, 0.6f, 16.0f);
	mDirLights[0].Direction = XMFLOAT3(0.707f, -0.707f, 0.0f);
 
	mDirLights[1].Ambient  = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLights[1].Diffuse  = XMFLOAT4(1.4f, 1.4f, 1.4f, 1.0f);
	mDirLights[1].Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 16.0f);
	mDirLights[1].Direction = XMFLOAT3(-0.707f, 0.0f, 0.707f);

	mBoxMat.Ambient  = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mBoxMat.Diffuse  = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mBoxMat.Specular = XMFLOAT4(0.6f, 0.6f, 0.6f, 16.0f);

	//edit
	mPhoneCaseMat.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mPhoneCaseMat.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mPhoneCaseMat.Specular = XMFLOAT4(0.6f, 0.6f, 0.6f, 16.0f);
}

CrateApp::~CrateApp()
{
	ReleaseCOM(mBoxVB);
	ReleaseCOM(mBoxIB);
	ReleaseCOM(mPhoneCaseVB);
	ReleaseCOM(mPhoneCaseIB);
	ReleaseCOM(mDiffuseMapSRV);

	Effects::DestroyAll();
	InputLayouts::DestroyAll();
}

bool CrateApp::Init()
{
	if(!D3DApp::Init())
		return false;

	// Must init Effects first since InputLayouts depend on shader signatures.
	Effects::InitAll(md3dDevice);
	InputLayouts::InitAll(md3dDevice);

	HR(D3DX11CreateShaderResourceViewFromFile(md3dDevice, 
		L"Textures/WoodCrate01.dds", 0, 0, &mDiffuseMapSRV, 0 ));

	HR(D3DX11CreateShaderResourceViewFromFile(md3dDevice,
		L"Textures/mobile.png", 0, 0, &mPhoneDiffuseMApSRV, 0));

	HR(D3DX11CreateShaderResourceViewFromFile(md3dDevice,
		L"Textures/Box.png", 0, 0, &mScreenDiffuseMapSRV, 0));
 
	BuildGeometryBuffers();
	BuildPhoneGeometryBuffers();
	D3D11_RASTERIZER_DESC wfd;										// wireframe description
	ZeroMemory(&wfd, sizeof(D3D11_RASTERIZER_DESC));				// will initialized all the structures in our description, will be set to 0 or false
	wfd.FillMode = D3D11_FILL_WIREFRAME;							// will render in a wireframe mode
	wfd.CullMode = D3D11_CULL_NONE;									// 
	wfd.DepthClipEnable = true;										// set to default value

	md3dDevice->CreateRasterizerState(&wfd, &mWireframeRS);
	return true;
}

void CrateApp::OnResize()
{
	D3DApp::OnResize();

	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f*MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&mProj, P);
}

void CrateApp::UpdateScene(float dt)
{
	// Convert Spherical to Cartesian coordinates.
	float x = mRadius*sinf(mPhi)*cosf(mTheta);
	float z = mRadius*sinf(mPhi)*sinf(mTheta);
	float y = mRadius*cosf(mPhi);

	mEyePosW = XMFLOAT3(x, y, z);

	// Build the view matrix.
	XMVECTOR pos    = XMVectorSet(x, y, z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up     = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX V = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&mView, V);
}

void CrateApp::DrawScene()
{
	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::LightSteelBlue));
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

	md3dImmediateContext->IASetInputLayout(InputLayouts::Basic32);
    md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
 
	UINT stride = sizeof(Vertex::Basic32);
    UINT offset = 0;
 
	XMMATRIX view  = XMLoadFloat4x4(&mView);
	XMMATRIX proj  = XMLoadFloat4x4(&mProj);
	XMMATRIX viewProj = view*proj;

	//md3dImmediateContext->RSSetState(mWireframeRS); // edit


	// Set per frame constants.
	Effects::BasicFX->SetDirLights(mDirLights);
	Effects::BasicFX->SetEyePosW(mEyePosW);
 
	ID3DX11EffectTechnique* activeTech = Effects::BasicFX->Light2TexTech;

    D3DX11_TECHNIQUE_DESC techDesc;
	activeTech->GetDesc( &techDesc );
    for(UINT p = 0; p < techDesc.Passes; ++p)
    {
		md3dImmediateContext->IASetVertexBuffers(0, 1, &mBoxVB, &stride, &offset);
		md3dImmediateContext->IASetIndexBuffer(mBoxIB, DXGI_FORMAT_R32_UINT, 0);

		// Draw the box.
		XMMATRIX world = XMLoadFloat4x4(&mBoxWorld);
		XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
		XMMATRIX worldViewProj = world*view*proj;

		Effects::BasicFX->SetWorld(world);
		Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::BasicFX->SetWorldViewProj(worldViewProj);
		Effects::BasicFX->SetTexTransform(XMLoadFloat4x4(&mTexTransform));
		Effects::BasicFX->SetMaterial(mBoxMat);
		Effects::BasicFX->SetDiffuseMap(mDiffuseMapSRV);

		activeTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
		md3dImmediateContext->DrawIndexed(mBoxIndexCount, mBoxIndexOffset, mBoxVertexOffset);

		// edit

		md3dImmediateContext->IASetVertexBuffers(0, 1, &mPhoneCaseVB, &stride, &offset);
		md3dImmediateContext->IASetIndexBuffer(mPhoneCaseIB, DXGI_FORMAT_R32_UINT, 0);

		// draw the phone
		world = XMLoadFloat4x4(&mPhoneCaseWorld);
		worldInvTranspose = MathHelper::InverseTranspose(world);
		worldViewProj = world * view*proj;

		Effects::BasicFX->SetWorld(world);
		Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::BasicFX->SetWorldViewProj(worldViewProj);
		Effects::BasicFX->SetTexTransform(XMLoadFloat4x4(&mTexTransform));
		Effects::BasicFX->SetMaterial(mPhoneCaseMat);
		Effects::BasicFX->SetDiffuseMap(mPhoneDiffuseMApSRV); 

		activeTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
		md3dImmediateContext->DrawIndexed(mPhoneCaseIndexCount, mPhoneCaseIndexOffset, mPhoneCaseVertexOffset);



		md3dImmediateContext->IASetVertexBuffers(0, 1, &mPhoneScreenVB, &stride, &offset);
		md3dImmediateContext->IASetIndexBuffer(mPhoneScreenIB, DXGI_FORMAT_R32_UINT, 0);

		// draw the phone
		world = XMLoadFloat4x4(&mPhoneCaseWorld);
		worldInvTranspose = MathHelper::InverseTranspose(world);
		worldViewProj = world * view*proj;

		Effects::BasicFX->SetWorld(world);
		Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::BasicFX->SetWorldViewProj(worldViewProj);
		Effects::BasicFX->SetTexTransform(XMLoadFloat4x4(&mTexTransform));
		Effects::BasicFX->SetMaterial(mPhoneCaseMat);
		Effects::BasicFX->SetDiffuseMap(mPhoneDiffuseMApSRV);

		activeTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
		md3dImmediateContext->DrawIndexed(mPhoneScreenIndexCount, mPhoneScreenIndexOffset, mphoneScreenVertexOffset);

	//	md3dImmediateContext->Draw(28, 0);
    }

	HR(mSwapChain->Present(0, 0));
}

void CrateApp::OnMouseDown(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	SetCapture(mhMainWnd);
}

void CrateApp::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void CrateApp::OnMouseMove(WPARAM btnState, int x, int y)
{
	if( (btnState & MK_LBUTTON) != 0 )
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f*static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(y - mLastMousePos.y));

		// Update angles based on input to orbit camera around box.
		mTheta += dx;
		mPhi   += dy;

		// Restrict the angle mPhi.
		mPhi = MathHelper::Clamp(mPhi, 0.1f, MathHelper::Pi-0.1f);
	}
	else if( (btnState & MK_RBUTTON) != 0 )
	{
		// Make each pixel correspond to 0.01 unit in the scene.
		float dx = 0.01f*static_cast<float>(x - mLastMousePos.x);
		float dy = 0.01f*static_cast<float>(y - mLastMousePos.y);

		// Update the camera radius based on input.
		mRadius += dx - dy;

		// Restrict the radius.
		mRadius = MathHelper::Clamp(mRadius, 1.0f, 15.0f);
	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}

void CrateApp::BuildGeometryBuffers()
{
	GeometryGenerator::MeshData box;
	GeometryGenerator::MeshData phoneCase; //edit
	GeometryGenerator::MeshData phoneScreen; // edit

	GeometryGenerator geoGen;
	geoGen.CreateBox(1.0f, 1.0f, 1.0f, box);

	// Cache the vertex offsets to each object in the concatenated vertex buffer.
	mBoxVertexOffset      = 0;
	// Cache the index count of each object.
	mBoxIndexCount      = box.Indices.size();
	// Cache the starting index for each object in the concatenated index buffer.
	mBoxIndexOffset      = 0;
	UINT totalVertexCount = box.Vertices.size();
	UINT totalIndexCount = mBoxIndexCount;

	//
	// Extract the vertex elements we are interested in and pack the
	// vertices of all the meshes into one vertex buffer.
	//

	std::vector<Vertex::Basic32> vertices(totalVertexCount);

	UINT k = 0;
	for(size_t i = 0; i < box.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos    = box.Vertices[i].Position;
		vertices[k].Normal = box.Vertices[i].Normal;
		vertices[k].Tex    = box.Vertices[i].TexC;
	}

    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(Vertex::Basic32) * totalVertexCount;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = &vertices[0];
    HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mBoxVB));

	//
	// Pack the indices of all the meshes into one index buffer.
	//

	std::vector<UINT> indices;
	indices.insert(indices.end(), box.Indices.begin(), box.Indices.end());
	indices.insert(indices.end(), phoneCase.Indices.begin(), phoneCase.Indices.end());

	D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.ByteWidth = sizeof(UINT) * totalIndexCount;
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = &indices[0];
    HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mBoxIB));


	// edit
	geoGen.CreateBox(2.0f, 4.0f, 0.2f, phoneCase); // edit
	mPhoneCaseVertexOffset = 0; // edit
	mPhoneCaseIndexCount = phoneCase.Indices.size(); // edit
	mPhoneCaseIndexOffset = 0; //edit
	UINT totalPhoneVertexCount = phoneCase.Vertices.size(); // edit
	UINT totalPhoneIndexCount = mPhoneCaseIndexCount; // edit

	std::vector<Vertex::Basic32> vertices2(totalPhoneVertexCount);

	UINT l = 0;
	for (size_t i = 0; i < phoneCase.Vertices.size(); ++i, ++l)
	{
		//edit: change position of the phone
		XMFLOAT3 p = phoneCase.Vertices[i].Position;

		p.z = -2.0f;

		XMFLOAT2 t = phoneCase.Vertices[i].TexC;

		vertices2[l].Pos = p;
		vertices2[l].Normal = phoneCase.Vertices[i].Normal;
		vertices2[l].Tex = t;
	}

	D3D11_BUFFER_DESC vbd2;
	vbd2.Usage = D3D11_USAGE_IMMUTABLE;
	vbd2.ByteWidth = sizeof(Vertex::Basic32) * totalPhoneVertexCount;
	vbd2.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd2.CPUAccessFlags = 0;
	vbd2.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData2;
	vinitData2.pSysMem = &vertices2[0];
	HR(md3dDevice->CreateBuffer(&vbd2, &vinitData2, &mPhoneCaseVB));

	//
	// Pack the indices of all the meshes into one index buffer.
	//

	std::vector<UINT> indices2;
	indices2.insert(indices2.end(), phoneCase.Indices.begin(), phoneCase.Indices.end());

	D3D11_BUFFER_DESC ibd2;
	ibd2.Usage = D3D11_USAGE_IMMUTABLE;
	ibd2.ByteWidth = sizeof(UINT) * totalPhoneIndexCount;
	ibd2.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd2.CPUAccessFlags = 0;
	ibd2.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA iinitData2;
	iinitData2.pSysMem = &indices2[0];
	HR(md3dDevice->CreateBuffer(&ibd2, &iinitData2, &mPhoneCaseIB));

	// edit

	geoGen.CreateBox(1.9f, 3.2f, 0.1f, phoneScreen);
	mphoneScreenVertexOffset = 0;
	mPhoneScreenIndexCount = phoneScreen.Indices.size();
	mPhoneScreenIndexOffset = 0;
	UINT totalPhoneScreenVertexCount = phoneScreen.Vertices.size();
	UINT totalPhoneScreenIndexCount = mPhoneScreenIndexCount;

	std::vector<Vertex::Basic32> vertices3(totalPhoneScreenVertexCount);
	UINT j = 0;

	for (size_t i = 0; i < phoneScreen.Vertices.size(); ++i, ++j)
	{
		//edit: change position of the phone
		XMFLOAT3 p = phoneScreen.Vertices[i].Position;

		p.z = -2.0001f;

		XMFLOAT2 t = phoneScreen.Vertices[i].TexC;
		if (i == 0 || i == 1)
			t.x = 0.35f;
		else if (i == 2 || i == 3)
			t.x = 0.66f;

		vertices3[j].Pos = p;
		vertices3[j].Normal = phoneScreen.Vertices[i].Normal;
		vertices3[j].Tex = t;
	}

	D3D11_BUFFER_DESC vbd3;
	vbd3.Usage = D3D11_USAGE_IMMUTABLE;
	vbd3.ByteWidth = sizeof(Vertex::Basic32) * totalPhoneScreenVertexCount;
	vbd3.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd3.CPUAccessFlags = 0;
	vbd3.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData3;
	vinitData3.pSysMem = &vertices3[0];
	HR(md3dDevice->CreateBuffer(&vbd3, &vinitData3, &mPhoneScreenVB));

	//
	// Pack the indices of all the meshes into one index buffer.
	//

	std::vector<UINT> indices3;
	indices3.insert(indices3.end(), phoneScreen.Indices.begin(), phoneScreen.Indices.end());

	D3D11_BUFFER_DESC ibd3;
	ibd3.Usage = D3D11_USAGE_IMMUTABLE;
	ibd3.ByteWidth = sizeof(UINT) * totalPhoneScreenIndexCount;
	ibd3.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd3.CPUAccessFlags = 0;
	ibd3.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA iinitData3;
	iinitData3.pSysMem = &indices2[0];
	HR(md3dDevice->CreateBuffer(&ibd3, &iinitData3, &mPhoneScreenIB));
}

void CrateApp::BuildPhoneGeometryBuffers()
{
	int const totalVertex = 28;
	Vertex::Basic32 v[totalVertex];
	// phone
	// back
	v[0] = Vertex::Basic32(+1.0f, -2.0f, -1.8f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	v[1] = Vertex::Basic32(+1.0f, 2.0f, -1.8f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);


	v[2] = Vertex::Basic32(-1.0f, 2.0f, -1.8f, -1.0f, 0.0f, 0.0f, .69f, 0.0f);
	v[3] = Vertex::Basic32(-1.0f, -2.0f, -1.8f, -1.0f, 0.0f, 0.0f, 0.69f, 1.0f);

	//left side
	v[4] = Vertex::Basic32(-1.0f, 2.0f, -1.8f, -1.0f, 0.0f, 0.0f, 0.69f, 1.0f);
	v[5] = Vertex::Basic32(-1.0f, 2.0f, -2.0f, -1.0f, 0.0f, 0.0f, 0.69f, 1.0f);

	v[6] = Vertex::Basic32(-1.0f, -2.0f, -2.0f, -1.0f, 0.0f, 0.0f, 0.69f, 1.0f);
	v[7] = Vertex::Basic32(-1.0f, -2.0f, -1.8f, -1.0f, 0.0f, 0.0f, 0.69f, 1.0f);

	//up side
	v[8] = Vertex::Basic32(1.0f, 2.0f, -1.8f, -1.0f, 0.0f, 0.0f, 0.69f, 1.0f);
	v[9] = Vertex::Basic32(1.0f, 2.0f, -2.0f, -1.0f, 0.0f, 0.0f, 0.69f, 1.0f);

	v[10] = Vertex::Basic32(-1.0f, 2.0f, -2.0f, -1.0f, 0.0f, 0.0f, 0.69f, 1.0f);
	v[11] = Vertex::Basic32(-1.0f, 2.0f, -1.8f, -1.0f, 0.0f, 0.0f, 0.69f, 1.0f);

	//right side
	v[12] = Vertex::Basic32(1.0f, -2.0f, -1.8f, -1.0f, 0.0f, 0.0f, 0.69f, 1.0f);
	v[13] = Vertex::Basic32(1.0f, -2.0f, -2.0f, -1.0f, 0.0f, 0.0f, 0.69f, 1.0f);

	v[14] = Vertex::Basic32(1.0f, 2.0f, -2.0f, -1.0f, 0.0f, 0.0f, 0.69f, 1.0f);
	v[15] = Vertex::Basic32(1.0f, 2.0f, -1.8f, -1.0f, 0.0f, 0.0f, 0.69f, 1.0f);

	//down side
	v[16] = Vertex::Basic32(-1.0f, -2.0f, -1.8f, -1.0f, 0.0f, 0.0f, 0.69f, 1.0f);
	v[17] = Vertex::Basic32(-1.0f, -2.0f, -2.0f, -1.0f, 0.0f, 0.0f, 0.69f, 1.0f);

	v[18] = Vertex::Basic32(1.0f, -2.0f, -2.0f, -1.0f, 0.0f, 0.0f, 0.69f, 1.0f);
	v[19] = Vertex::Basic32(1.0f, -2.0f, -1.8f, -1.0f, 0.0f, 0.0f, 0.69f, 1.0f);

	//front
	v[20] = Vertex::Basic32(+1.0f, 2.0f, -2.0f, -1.0f, 0.0f, 0.0f, .33f, 0.0f);
	v[21] = Vertex::Basic32(+1.0f, -2.0f, -2.0f, -1.0f, 0.0f, 0.0f, 0.33f, 1.0f);

	v[22] = Vertex::Basic32(-1.0f, -2.0f, -2.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[23] = Vertex::Basic32(-1.0f, 2.0f, -2.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	
	// screen
	v[24] = Vertex::Basic32(-.95f, -1.65f, -2.0001f, 0.0f, 0.0f, -1.0f, .35f, 1.0f);
	v[25] = Vertex::Basic32(-.95f, 1.5f, -2.0001f, 0.0f, 0.0f, -1.0f, .35f, 0.0f);

	v[26] = Vertex::Basic32(+.95f, 1.5f, -2.0001f, 0.0f, 0.0f, -1.0f, 0.66f, 0.0f);
	v[27] = Vertex::Basic32(+.95f, -1.65f, -2.0001f, 0.0f, 0.0f, -1.0f, 0.66f, 1.0f);

	

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::Basic32) * totalVertex;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = v;
	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mPhoneCaseVB));
} 
