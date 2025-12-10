#include "ResizeBuffers.h"

#include "../hack.h"

HRESULT __fastcall ResizeBuffers(IDXGISwapChain* pSwapChain, std::uint32_t nBufferCount, std::uint32_t nWidth, std::uint32_t nHeight, DXGI_FORMAT newFormat, std::uint32_t nFlags) {
    
    HRESULT hr = hkResizeBuffers.original()(pSwapChain, nBufferCount, nWidth, nHeight, newFormat, nFlags);
    if (SUCCEEDED(hr))
        gHack.CreateRenderTarget();

    return hr;
}
