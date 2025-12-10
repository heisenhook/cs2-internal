#include "CreateSwapChain.h"

#include "../hack.h"

HRESULT __fastcall CreateSwapChain(IDXGIFactory* pFactory, IUnknown* pDevice, DXGI_SWAP_CHAIN_DESC* pDesc, IDXGISwapChain** ppSwapChain) {
    gHack.DestroyRenderTarget();
    return hkCreateSwapChain.original()(pFactory, pDevice, pDesc, ppSwapChain);
}