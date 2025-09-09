#pragma once
#include "../../CommonUtilities/win/WinAPI.h"
#include <d3d11.h>

#include "../Context.h"

namespace GfxLayer::Extension::Quad
{
	struct ConstantBuffer
	{
		float Color[4];
	};

	struct Vertex
	{
		float x, y, z;
	};

	extern const Vertex Vertices[4];
	extern const unsigned Indices[6];
	extern const char* pVertexShader;
	extern const char* pPixelShader;

	void CompileShader(const char* pShaderSource, const char* pEntryPoint, const char* pTarget, ID3DBlob** ppBlob);
}