#include "Quad.h"
#include "../../CommonUtilities/win/WinAPI.h"
#include <wrl.h>
#include "../Logging.h"

#include <d3dcompiler.h>


namespace GfxLayer::Extension::Quad
{
	const Vertex Vertices[4] =
	{
		{ -1.0f,  1.0f, 0.0f },
		{  1.0f,  1.0f, 0.0f },
		{  1.0f, -1.0f, 0.0f },
		{ -1.0f, -1.0f, 0.0f },
	};

	const unsigned Indices[6] =
	{
		0, 1, 2,
		0, 2, 3,
	};

	const char* pVertexShader = R"(
	struct VS_INPUT
	{
		float3 Pos : POSITION;
	};

	struct PS_INPUT
	{
		float4 Pos : SV_POSITION;
	};

	PS_INPUT VS(VS_INPUT input)
	{
		PS_INPUT output = (PS_INPUT)0;
		output.Pos = float4(input.Pos, 1.0f);
		return output;
	}
	)";

	const char* pPixelShader = R"(
	cbuffer ConstantBuffer : register(b0)
	{
		float4 Color;
	};

	struct PS_INPUT
	{
		float4 Pos : SV_POSITION;
	};

	float4 PS(PS_INPUT input) : SV_Target
	{
		return Color;
	}
	)";

	void CompileShader(const char* pShaderSource, const char* pEntryPoint, const char* pTarget, ID3DBlob** ppBlob)
	{
		Microsoft::WRL::ComPtr<ID3DBlob> pErrorBlob = nullptr;
		HRESULT hr = D3DCompile(pShaderSource, strlen(pShaderSource), NULL, NULL, NULL, pEntryPoint, pTarget, 0, 0, ppBlob, &pErrorBlob);
		if (FAILED(hr) && pErrorBlob)
		{
			LOGE << "SHADER COMPILATION ERROR:";
			LOGE << (char*)pErrorBlob->GetBufferPointer();
		}
	}
}