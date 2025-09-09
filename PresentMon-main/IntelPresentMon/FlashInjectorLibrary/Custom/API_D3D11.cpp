#include "API_D3D11.h"
#include "../Context.h"

namespace GfxLayer
{
    void WrapID3D11Device(REFIID riid, void** ppObject)
    {
        auto& ctx = Context::GetInstance();
        auto** ppObjectToWrap = reinterpret_cast<IUnknown**>(ppObject);
        auto* pUnknownWrapper = ctx.FindWrapper(*ppObjectToWrap);
        if (!pUnknownWrapper)
        {
            pUnknownWrapper = new ID3D11Device_Wrapper(riid, *ppObjectToWrap);
            ctx.AddWrapper(pUnknownWrapper);
        }
        (*ppObject) = pUnknownWrapper;
    }

    void WrapID3D11Device_NoStore(REFIID riid, void** ppObject)
    {
        auto** ppObjectToWrap = reinterpret_cast<IUnknown**>(ppObject);
        (*ppObject) = new ID3D11Device_Wrapper(riid, *ppObjectToWrap);
    }

    HRESULT STDMETHODCALLTYPE ID3D11Device_Wrapper::CreateBuffer(const D3D11_BUFFER_DESC* pDesc, const D3D11_SUBRESOURCE_DATA* pInitialData, ID3D11Buffer** ppBuffer)
    {
        return GetWrappedObjectAs<ID3D11Device>()->CreateBuffer(pDesc, pInitialData, ppBuffer);
    }

    HRESULT STDMETHODCALLTYPE ID3D11Device_Wrapper::CreateTexture1D(const D3D11_TEXTURE1D_DESC* pDesc, const D3D11_SUBRESOURCE_DATA* pInitialData, ID3D11Texture1D** ppTexture1D)
    {
        return GetWrappedObjectAs<ID3D11Device>()->CreateTexture1D(pDesc, pInitialData, ppTexture1D);
    }

    HRESULT STDMETHODCALLTYPE ID3D11Device_Wrapper::CreateTexture2D(const D3D11_TEXTURE2D_DESC* pDesc, const D3D11_SUBRESOURCE_DATA* pInitialData, ID3D11Texture2D** ppTexture2D)
    {
        return GetWrappedObjectAs<ID3D11Device>()->CreateTexture2D(pDesc, pInitialData, ppTexture2D);
    }

    HRESULT STDMETHODCALLTYPE ID3D11Device_Wrapper::CreateTexture3D(const D3D11_TEXTURE3D_DESC* pDesc, const D3D11_SUBRESOURCE_DATA* pInitialData, ID3D11Texture3D** ppTexture3D)
    {
        return GetWrappedObjectAs<ID3D11Device>()->CreateTexture3D(pDesc, pInitialData, ppTexture3D);
    }

    HRESULT STDMETHODCALLTYPE ID3D11Device_Wrapper::CreateShaderResourceView(ID3D11Resource* pResource, const D3D11_SHADER_RESOURCE_VIEW_DESC* pDesc, ID3D11ShaderResourceView** ppSRView)
    {
        return GetWrappedObjectAs<ID3D11Device>()->CreateShaderResourceView(pResource, pDesc, ppSRView);
    }

    HRESULT STDMETHODCALLTYPE ID3D11Device_Wrapper::CreateUnorderedAccessView(ID3D11Resource* pResource, const D3D11_UNORDERED_ACCESS_VIEW_DESC* pDesc, ID3D11UnorderedAccessView** ppUAView)
    {
        return GetWrappedObjectAs<ID3D11Device>()->CreateUnorderedAccessView(pResource, pDesc, ppUAView);
    }

    HRESULT STDMETHODCALLTYPE ID3D11Device_Wrapper::CreateRenderTargetView(ID3D11Resource* pResource, const D3D11_RENDER_TARGET_VIEW_DESC* pDesc, ID3D11RenderTargetView** ppRTView)
    {
        return GetWrappedObjectAs<ID3D11Device>()->CreateRenderTargetView(pResource, pDesc, ppRTView);
    }

    HRESULT STDMETHODCALLTYPE ID3D11Device_Wrapper::CreateDepthStencilView(ID3D11Resource* pResource, const D3D11_DEPTH_STENCIL_VIEW_DESC* pDesc, ID3D11DepthStencilView** ppDepthStencilView)
    {
        return GetWrappedObjectAs<ID3D11Device>()->CreateDepthStencilView(pResource, pDesc, ppDepthStencilView);
    }

    HRESULT STDMETHODCALLTYPE ID3D11Device_Wrapper::CreateInputLayout(const D3D11_INPUT_ELEMENT_DESC* pInputElementDescs, UINT NumElements, const void* pShaderBytecodeWithInputSignature, SIZE_T BytecodeLength, ID3D11InputLayout** ppInputLayout)
    {
        return GetWrappedObjectAs<ID3D11Device>()->CreateInputLayout(pInputElementDescs, NumElements, pShaderBytecodeWithInputSignature, BytecodeLength, ppInputLayout);
    }

    HRESULT STDMETHODCALLTYPE ID3D11Device_Wrapper::CreateVertexShader(const void* pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage* pClassLinkage, ID3D11VertexShader** ppVertexShader)
    {
        return GetWrappedObjectAs<ID3D11Device>()->CreateVertexShader(pShaderBytecode, BytecodeLength, pClassLinkage, ppVertexShader);
    }

    HRESULT STDMETHODCALLTYPE ID3D11Device_Wrapper::CreateGeometryShader(const void* pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage* pClassLinkage, ID3D11GeometryShader** ppGeometryShader)
    {
        return GetWrappedObjectAs<ID3D11Device>()->CreateGeometryShader(pShaderBytecode, BytecodeLength, pClassLinkage, ppGeometryShader);
    }

    HRESULT STDMETHODCALLTYPE ID3D11Device_Wrapper::CreateGeometryShaderWithStreamOutput(const void* pShaderBytecode, SIZE_T BytecodeLength, const D3D11_SO_DECLARATION_ENTRY* pSODeclaration, UINT NumEntries, const UINT* pBufferStrides, UINT NumStrides, UINT RasterizedStream, ID3D11ClassLinkage* pClassLinkage, ID3D11GeometryShader** ppGeometryShader)
    {
        return GetWrappedObjectAs<ID3D11Device>()->CreateGeometryShaderWithStreamOutput(pShaderBytecode, BytecodeLength, pSODeclaration, NumEntries, pBufferStrides, NumStrides, RasterizedStream, pClassLinkage, ppGeometryShader);
    }

    HRESULT STDMETHODCALLTYPE ID3D11Device_Wrapper::CreatePixelShader(const void* pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage* pClassLinkage, ID3D11PixelShader** ppPixelShader)
    {
        return GetWrappedObjectAs<ID3D11Device>()->CreatePixelShader(pShaderBytecode, BytecodeLength, pClassLinkage, ppPixelShader);
    }

    HRESULT STDMETHODCALLTYPE ID3D11Device_Wrapper::CreateHullShader(const void* pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage* pClassLinkage, ID3D11HullShader** ppHullShader)
    {
        return GetWrappedObjectAs<ID3D11Device>()->CreateHullShader(pShaderBytecode, BytecodeLength, pClassLinkage, ppHullShader);
    }

    HRESULT STDMETHODCALLTYPE ID3D11Device_Wrapper::CreateDomainShader(const void* pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage* pClassLinkage, ID3D11DomainShader** ppDomainShader)
    {
        return GetWrappedObjectAs<ID3D11Device>()->CreateDomainShader(pShaderBytecode, BytecodeLength, pClassLinkage, ppDomainShader);
    }

    HRESULT STDMETHODCALLTYPE ID3D11Device_Wrapper::CreateComputeShader(const void* pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage* pClassLinkage, ID3D11ComputeShader** ppComputeShader)
    {
        return GetWrappedObjectAs<ID3D11Device>()->CreateComputeShader(pShaderBytecode, BytecodeLength, pClassLinkage, ppComputeShader);
    }

    HRESULT STDMETHODCALLTYPE ID3D11Device_Wrapper::CreateClassLinkage(ID3D11ClassLinkage** ppLinkage)
    {
        return GetWrappedObjectAs<ID3D11Device>()->CreateClassLinkage(ppLinkage);
    }

    HRESULT STDMETHODCALLTYPE ID3D11Device_Wrapper::CreateBlendState(const D3D11_BLEND_DESC* pBlendStateDesc, ID3D11BlendState** ppBlendState)
    {
        return GetWrappedObjectAs<ID3D11Device>()->CreateBlendState(pBlendStateDesc, ppBlendState);
    }

    HRESULT STDMETHODCALLTYPE ID3D11Device_Wrapper::CreateDepthStencilState(const D3D11_DEPTH_STENCIL_DESC* pDepthStencilDesc, ID3D11DepthStencilState** ppDepthStencilState)
    {
        return GetWrappedObjectAs<ID3D11Device>()->CreateDepthStencilState(pDepthStencilDesc, ppDepthStencilState);
    }

    HRESULT STDMETHODCALLTYPE ID3D11Device_Wrapper::CreateRasterizerState(const D3D11_RASTERIZER_DESC* pRasterizerDesc, ID3D11RasterizerState** ppRasterizerState)
    {
        return GetWrappedObjectAs<ID3D11Device>()->CreateRasterizerState(pRasterizerDesc, ppRasterizerState);
    }

    HRESULT STDMETHODCALLTYPE ID3D11Device_Wrapper::CreateSamplerState(const D3D11_SAMPLER_DESC* pSamplerDesc, ID3D11SamplerState** ppSamplerState)
    {
        return GetWrappedObjectAs<ID3D11Device>()->CreateSamplerState(pSamplerDesc, ppSamplerState);
    }

    HRESULT STDMETHODCALLTYPE ID3D11Device_Wrapper::CreateQuery(const D3D11_QUERY_DESC* pQueryDesc, ID3D11Query** ppQuery)
    {
        return GetWrappedObjectAs<ID3D11Device>()->CreateQuery(pQueryDesc, ppQuery);
    }

    HRESULT STDMETHODCALLTYPE ID3D11Device_Wrapper::CreatePredicate(const D3D11_QUERY_DESC* pPredicateDesc, ID3D11Predicate** ppPredicate)
    {
        return GetWrappedObjectAs<ID3D11Device>()->CreatePredicate(pPredicateDesc, ppPredicate);
    }

    HRESULT STDMETHODCALLTYPE ID3D11Device_Wrapper::CreateCounter(const D3D11_COUNTER_DESC* pCounterDesc, ID3D11Counter** ppCounter)
    {
        return GetWrappedObjectAs<ID3D11Device>()->CreateCounter(pCounterDesc, ppCounter);
    }

    HRESULT STDMETHODCALLTYPE ID3D11Device_Wrapper::CreateDeferredContext(UINT ContextFlags, ID3D11DeviceContext** ppDeferredContext)
    {
        return GetWrappedObjectAs<ID3D11Device>()->CreateDeferredContext(ContextFlags, ppDeferredContext);
    }

    HRESULT STDMETHODCALLTYPE ID3D11Device_Wrapper::OpenSharedResource(HANDLE hResource, REFIID ReturnedInterface, void** ppResource)
    {
        return GetWrappedObjectAs<ID3D11Device>()->OpenSharedResource(hResource, ReturnedInterface, ppResource);
    }

    HRESULT STDMETHODCALLTYPE ID3D11Device_Wrapper::CheckFormatSupport(DXGI_FORMAT Format, UINT* pFormatSupport)
    {
        return GetWrappedObjectAs<ID3D11Device>()->CheckFormatSupport(Format, pFormatSupport);
    }

    HRESULT STDMETHODCALLTYPE ID3D11Device_Wrapper::CheckMultisampleQualityLevels(DXGI_FORMAT Format, UINT SampleCount, UINT* pNumQualityLevels)
    {
        return GetWrappedObjectAs<ID3D11Device>()->CheckMultisampleQualityLevels(Format, SampleCount, pNumQualityLevels);
    }

    void STDMETHODCALLTYPE ID3D11Device_Wrapper::CheckCounterInfo(D3D11_COUNTER_INFO* pCounterInfo)
    {
        GetWrappedObjectAs<ID3D11Device>()->CheckCounterInfo(pCounterInfo);
    }

    HRESULT STDMETHODCALLTYPE ID3D11Device_Wrapper::CheckCounter(const D3D11_COUNTER_DESC* pDesc, D3D11_COUNTER_TYPE* pType, UINT* pActiveCounters, char* szName, UINT* pNameLength, char* szUnits, UINT* pUnitsLength, char* szDescription, UINT* pDescriptionLength)
    {
        return GetWrappedObjectAs<ID3D11Device>()->CheckCounter(pDesc, pType, pActiveCounters, szName, pNameLength, szUnits, pUnitsLength, szDescription, pDescriptionLength);
    }

    HRESULT STDMETHODCALLTYPE ID3D11Device_Wrapper::CheckFeatureSupport(D3D11_FEATURE Feature, void* pFeatureSupportData, UINT FeatureSupportDataSize)
    {
        return GetWrappedObjectAs<ID3D11Device>()->CheckFeatureSupport(Feature, pFeatureSupportData, FeatureSupportDataSize);
    }

    HRESULT STDMETHODCALLTYPE ID3D11Device_Wrapper::GetPrivateData(REFGUID guid, UINT* pDataSize, void* pData)
    {
        return GetWrappedObjectAs<ID3D11Device>()->GetPrivateData(guid, pDataSize, pData);
    }

    HRESULT STDMETHODCALLTYPE ID3D11Device_Wrapper::SetPrivateData(REFGUID guid, UINT DataSize, const void* pData)
    {
        return GetWrappedObjectAs<ID3D11Device>()->SetPrivateData(guid, DataSize, pData);
    }

    HRESULT STDMETHODCALLTYPE ID3D11Device_Wrapper::SetPrivateDataInterface(REFGUID guid, const IUnknown* pData)
    {
        return GetWrappedObjectAs<ID3D11Device>()->SetPrivateDataInterface(guid, pData);
    }

    D3D_FEATURE_LEVEL STDMETHODCALLTYPE ID3D11Device_Wrapper::GetFeatureLevel()
    {
        return GetWrappedObjectAs<ID3D11Device>()->GetFeatureLevel();
    }

    UINT STDMETHODCALLTYPE ID3D11Device_Wrapper::GetCreationFlags()
    {
        return GetWrappedObjectAs<ID3D11Device>()->GetCreationFlags();
    }

    HRESULT STDMETHODCALLTYPE ID3D11Device_Wrapper::GetDeviceRemovedReason()
    {
        return GetWrappedObjectAs<ID3D11Device>()->GetDeviceRemovedReason();
    }

    void STDMETHODCALLTYPE ID3D11Device_Wrapper::GetImmediateContext(ID3D11DeviceContext** ppImmediateContext)
    {
        GetWrappedObjectAs<ID3D11Device>()->GetImmediateContext(ppImmediateContext);
    }

    HRESULT STDMETHODCALLTYPE ID3D11Device_Wrapper::SetExceptionMode(UINT RaiseFlags)
    {
        return GetWrappedObjectAs<ID3D11Device>()->SetExceptionMode(RaiseFlags);
    }

    UINT STDMETHODCALLTYPE ID3D11Device_Wrapper::GetExceptionMode()
    {
        return GetWrappedObjectAs<ID3D11Device>()->GetExceptionMode();
    }
} // GfxLayer