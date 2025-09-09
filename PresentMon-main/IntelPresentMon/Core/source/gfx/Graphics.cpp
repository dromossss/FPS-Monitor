// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#include "Graphics.h"
#include "Exception.h"
#include <Core/source/infra/Logging.h>
#include <CommonUtilities/log/HrLogger.h>

#pragma comment(lib, "dxgi")
#pragma comment(lib, "d3d11")
#pragma comment(lib, "d2d1")
#pragma comment(lib, "dcomp")
#pragma comment(lib, "dwrite")


namespace p2c::gfx
{
    using namespace ::pmon::util;

	Graphics::Graphics(HWND hWnd, DimensionsI dimensions, float upscaleFactor, bool enableTearing, bool enableAlpha)
        :
        dims{ dimensions },
        enableAlpha{ enableAlpha }
	{
        // Direct3D 11 stuff
        if (auto hr = D3D11CreateDevice(
            nullptr,    // Adapter
            D3D_DRIVER_TYPE_HARDWARE,
            nullptr,    // Module
            D3D11_CREATE_DEVICE_BGRA_SUPPORT | (IS_DEBUG ? D3D11_CREATE_DEVICE_DEBUG : 0),
            nullptr, 0, // Highest available feature level
            D3D11_SDK_VERSION,
            &pDevice,
            nullptr,    // Actual feature level
            &pContext3d// Device context
        ); FAILED(hr))
        {
            pmlog_error().hr(hr);
        }

        ComPtr<IDXGIDevice> pDxgiDevice;
        if (auto hr = pDevice.As(&pDxgiDevice); FAILED(hr))
        {
            pmlog_error().hr(hr);
        }

        ComPtr<IDXGIFactory2> pDxgiFactory;
        if (auto hr = CreateDXGIFactory2(
            IS_DEBUG ? DXGI_CREATE_FACTORY_DEBUG : 0,
            IID_PPV_ARGS(&pDxgiFactory)
        ); FAILED(hr))
        {
            pmlog_error().hr(hr);
        }

        if (enableTearing) {
            // check for tearing (vrr) support 
            try {
                BOOL tearing{};
                ComPtr<IDXGIFactory5> factory5;
                if (auto hr = pDxgiFactory.As(&factory5); FAILED(hr)) {
                    throw Except<GraphicsException>();
                }
                if (auto hr = factory5->CheckFeatureSupport(
                    DXGI_FEATURE_PRESENT_ALLOW_TEARING, &tearing, sizeof(tearing)
                ); FAILED(hr)) {
                    throw Except<GraphicsException>();
                }
                tearingActive = tearing != 0;
                if (tearingActive) {
                    pmlog_info("Tearing graphics support enabled");
                }
                else {
                    pmlog_warn("Tearing requested but not supported, disabling");
                }
            }
            catch (...) {
                pmlog_warn("Failed checking tearing support, disabling tearing");
            }
        }

        if (!enableAlpha) {
            pmlog_info("Alpha composition disabled");
        }

        {
            // set swap chain configuration
            DXGI_SWAP_CHAIN_DESC1 description{};
            description.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
            description.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            description.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
            description.BufferCount = 2;
            description.SampleDesc.Count = 1;
            description.AlphaMode = enableAlpha ? DXGI_ALPHA_MODE_PREMULTIPLIED : DXGI_ALPHA_MODE_IGNORE;
            description.Width = dims.width;
            description.Height = dims.height;
            description.Flags = tearingActive ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

            if (auto hr = pDxgiFactory->CreateSwapChainForComposition(
                pDxgiDevice.Get(),
                &description,
                nullptr, // Don�t restrict
                &pSwapChain
            ); FAILED(hr))
            {
                pmlog_error().hr(hr);
            }
        }

        
        // DirectComposition stuff
        if (auto hr = DCompositionCreateDevice(
            pDxgiDevice.Get(),
            IID_PPV_ARGS(&pCompositionDevice)
        ); FAILED(hr))
        {
            pmlog_error().hr(hr);
        }

        if (auto hr = pCompositionDevice->CreateTargetForHwnd(
            hWnd,
            TRUE, // Top most
            &pCompositionTarget
        ); FAILED(hr))
        {
            pmlog_error().hr(hr);
        }

        if (auto hr = pCompositionDevice->CreateVisual(&pCompositionVisual); FAILED(hr))
        {
            pmlog_error().hr(hr);
        }

        if (auto hr = pCompositionVisual->SetContent(pSwapChain.Get()); FAILED(hr))
        {
            pmlog_error().hr(hr);
        }
        if (upscaleFactor != 1.f) {
            if (auto hr = pCompositionVisual->SetTransform(D2D1::Matrix3x2F::Scale(upscaleFactor, upscaleFactor)); FAILED(hr))
            {
                pmlog_error().hr(hr);
            }
        }

        if (auto hr = pCompositionTarget->SetRoot(pCompositionVisual.Get()); FAILED(hr))
        {
            pmlog_error().hr(hr);
        }

        if (auto hr = pCompositionDevice->Commit(); FAILED(hr))
        {
            pmlog_error().hr(hr);
        }


        // Direct2D stuff
        // Create a single-threaded Direct2D factory with debugging information
        if (auto hr = D2D1CreateFactory(
            D2D1_FACTORY_TYPE_SINGLE_THREADED,
            IS_DEBUG ? D2D1_FACTORY_OPTIONS{ D2D1_DEBUG_LEVEL_INFORMATION } : D2D1_FACTORY_OPTIONS{},
            pFactory2d.GetAddressOf()
        ); FAILED(hr))
        {
            pmlog_error().hr(hr);
        }

        // Create the Direct2D device that links back to the Direct3D device
        ComPtr<ID2D1Device2> pDevice2d;
        if (auto hr = pFactory2d->CreateDevice(
            pDxgiDevice.Get(),
            &pDevice2d
        ); FAILED(hr))
        {
            pmlog_error().hr(hr);
        }

        // Create the Direct2D device context that exposes drawing commands
        if (auto hr = pDevice2d->CreateDeviceContext(
            D2D1_DEVICE_CONTEXT_OPTIONS_NONE, // TODO: try D2D1_DEVICE_CONTEXT_OPTIONS_ENABLE_MULTITHREADED_OPTIMIZATIONS
            &pContext2d
        ); FAILED(hr))
        {
            pmlog_error().hr(hr);
        }


        // DirectWrite stuff
        if (auto hr = DWriteCreateFactory(
            DWRITE_FACTORY_TYPE_SHARED,
            __uuidof(IDWriteFactory),
            (IUnknown**)&pWriteFactory
        ); FAILED(hr))
        {
            pmlog_error().hr(hr);
        }

        CreateBackbufferDependentResources_();

        // fastline
        fastRenderer = impl::FastRenderer(*pDevice, dims);
    }

    Graphics::~Graphics()
    {
        // d2d debug layer will break on errors.
        // if exception happens before endDraw, destruction of context will trigger error break
        // and get in the way of our error handling mechanisms,
        // so we pre-emptively call EndDraw if we're unwinding in d2d debug layer and there is a context
        if (std::uncaught_exceptions() > 0 && IS_DEBUG && pContext2d)
        {
            pContext2d->EndDraw();
        }
    }

    void Graphics::BeginFrame()
    {
        static const float clearColor[] = { 0.f, 0.f, 0.f, 0.f };
        pContext3d->ClearRenderTargetView(pTarget.Get(), clearColor);
        pContext3d->OMSetRenderTargets(1, pTarget.GetAddressOf(), nullptr);
        fastRenderer->StartFrame(*pContext3d);
        pContext2d->BeginDraw();
    }

    void Graphics::EndFrame()
    {
        if (auto hr = pContext2d->EndDraw(); FAILED(hr))
        {
            pmlog_error().hr(hr);
        }

        fastRenderer->EndFrame(*pContext3d);

        {
            const UINT interval = tearingActive ? 0 : 1;
            const UINT flags = tearingActive ? DXGI_PRESENT_ALLOW_TEARING : 0;
            pmlog_hr << pSwapChain->Present(interval, flags);
        }

        fastRenderer->ResizeGeometryBuffersIfNecessary(*pDevice);
    }

    const DimensionsI& Graphics::GetDimensions() const
    {
        return dims;
    }

    void Graphics::Resize(DimensionsI dimensions)
    {
        dims = dimensions;
        FreeBackbufferDependentResources_();
        const UINT flags = tearingActive ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;
        if (auto hr = pSwapChain->ResizeBuffers(2, dims.width, dims.height, DXGI_FORMAT_UNKNOWN, flags); FAILED(hr)) {
            pmlog_warn("Failed to resize buffers").hr(hr);
        }
        CreateBackbufferDependentResources_();
        fastRenderer->Resize(dims);
    }


    // fastdraw interface
    void Graphics::FastLineBatchStart(const RectI& clip, bool aa)
    {
#ifdef _DEBUG
        if (!Rect{ {}, dims }.Contains(clip))
        {
            pmlog_warn("scissor rect outside window");
        }
#endif
        fastRenderer->StartLineBatch(clip, aa);
    }

    void Graphics::FastTriangleBatchStart(const RectI& clip)
    {
#ifdef _DEBUG
        if (!Rect{ {}, dims }.Contains(clip))
        {
            pmlog_warn("scissor rect outside window");
        }
#endif
        fastRenderer->StartTriangleBatch(clip);
    }

    void Graphics::FastBatchEnd()
    {
        fastRenderer->EndBatch();
    }

    void Graphics::FastLineStart(Vec2 pos, Color color)
    {
        fastRenderer->StartLine(pos, color);
    }

    void Graphics::FastLineAdd(Vec2 pos)
    {
        fastRenderer->AddVertex(pos);
    }

    void Graphics::FastLineEnd(Vec2 pos)
    {
        fastRenderer->EndLine(pos);
    }

    void Graphics::FastPeakStart(Vec2 top, Vec2 bottom, Color color)
    {
        fastRenderer->StartPeakChain(top, bottom, color);
    }

    void Graphics::FastPeakAdd(Vec2 top, Vec2 bottom)
    {
        fastRenderer->AddPeakChain(top, bottom);
    }

    void Graphics::FastPeakEnd(Vec2 top, Vec2 bottom)
    {
        fastRenderer->EndPeakChain(top, bottom);
    }

    void Graphics::FastRectEmit(const Rect& rect, Color color)
    {
        fastRenderer->EmitRect(rect, color);
    }

    void Graphics::FastLineRectTopEmit(const Rect& rect, Color color)
    {
        fastRenderer->EmitLineRectTop(rect, color);
    }

    void Graphics::FreeBackbufferDependentResources_()
    {
        pTarget.Reset();
        pContext2d->SetTarget(nullptr);
    }

    void Graphics::CreateBackbufferDependentResources_()
    {
        // getting access to that sweet backbuffer
        ComPtr<ID3D11Texture2D> pBackBuffer;
        pmlog_hr << pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));

        // get information from texture about dimensions
        D3D11_TEXTURE2D_DESC textureDesc;
        pBackBuffer->GetDesc(&textureDesc);

        // create the target view on the texture
        D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
        rtvDesc.Format = textureDesc.Format;
        rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
        rtvDesc.Texture2D.MipSlice = 0;

        pmlog_hr << pDevice->CreateRenderTargetView(pBackBuffer.Get(), &rtvDesc, &pTarget);

        // Retrieve the swap chain's back buffer
        ComPtr<IDXGISurface2> pSurface;
        pmlog_hr << pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pSurface));

        {
            // Create a Direct2D bitmap that points to the swap chain surface
            D2D1_BITMAP_PROPERTIES1 properties = {};
            properties.pixelFormat.alphaMode = enableAlpha ? D2D1_ALPHA_MODE_PREMULTIPLIED : D2D1_ALPHA_MODE_IGNORE;
            properties.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
            properties.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;

            ComPtr<ID2D1Bitmap1> pBitmap;
            pmlog_hr << pContext2d->CreateBitmapFromDxgiSurface(
                pSurface.Get(),
                properties,
                &pBitmap
            );

            // Point the device context to the bitmap for rendering
            pContext2d->SetTarget(pBitmap.Get());
        }
    }

    void Graphics::FastLineRectEmit(const Rect& rect, Color color)
    {
        fastRenderer->EmitLineRect(rect, color);
    }
}