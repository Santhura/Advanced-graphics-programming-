//***************************************************************************************
// RenderStates.h by Frank Luna (C) 2011 All Rights Reserved.
//   
// Defines render state objects.  
//***************************************************************************************

/*#ifndef RENDERSTATES_H
#define RENDERSTATES_H

#include "d3dUtil.h"

class RenderStates
{
public:
	static void InitAll(ID3D11Device* device);
	static void DestroyAll();

	// Rasterizer states
	static ID3D11RasterizerState* WireframeRS;
	static ID3D11RasterizerState* NoCullRS;
	static ID3D11RasterizerState* CullClockwiseRS;

	// Blend states
	static ID3D11BlendState* AlphaToCoverageBS;

	static ID3D11BlendState* AlphaBlending;
	static ID3D11BlendState* AdditiveBlending;
	static ID3D11BlendState* MultiplicativeBlending;
	static ID3D11BlendState* MultiplicativeBlendingX2;

	static ID3D11BlendState* NoRenderTargetWritesBS;

	// Depth/stencil states
	static ID3D11DepthStencilState* MarkMirrorDSS;
	static ID3D11DepthStencilState* DrawReflectionDSS;
	static ID3D11DepthStencilState* NoDoubleBlendDSS;
};

#endif // RENDERSTATES_H*/


//***************************************************************************************
// RenderStates.h by Frank Luna (C) 2011 All Rights Reserved.
//   
// Defines render state objects.  
//***************************************************************************************

#ifndef RENDERSTATES_H
#define RENDERSTATES_H

#include "d3dUtil.h"

class RenderStates
{
public:
	static void InitAll(ID3D11Device* device);
	static void DestroyAll();

	// Rasterizer states
	static ID3D11RasterizerState* WireframeRS;
	static ID3D11RasterizerState* NoCullRS;
	static ID3D11RasterizerState* CullClockwiseRS;

	// Blend states
	static ID3D11BlendState* AlphaToCoverageBS;
	static ID3D11BlendState* TransparentBS;
	static ID3D11BlendState* NoRenderTargetWritesBS;

	// Eigen state
	static ID3D11BlendState* TransparentBSMulti;
	static ID3D11BlendState* TransparentBSAdditive;

	// Depth/stencil states
	static ID3D11DepthStencilState* MarkMirrorDSS;
	static ID3D11DepthStencilState* DrawReflectionDSS;
	static ID3D11DepthStencilState* NoDoubleBlendDSS;
};

#endif // RENDERSTATES_H

/*
//***************************************************************************************
// RenderStates.h by Frank Luna (C) 2011 All Rights Reserved.
//
// Defines render state objects.
//***************************************************************************************

#ifndef RENDERSTATES_H
#define RENDERSTATES_H

#include "d3dUtil.h"

class RenderStates
{
public:
static void InitAll(ID3D11Device* device);
static void DestroyAll();

static ID3D11RasterizerState* WireframeRS;
static ID3D11RasterizerState* NoCullRS;

static ID3D11BlendState* AlphaToCoverageBS;
static ID3D11BlendState* TransparentBS;
};

#endif // RENDERSTATES_H
*/