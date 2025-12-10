#pragma once
#include <util\hook.h>
#include <cstdint>
#include <d3d11.h>

extern HRESULT __fastcall CreateSwapChain(IDXGIFactory* pFactory, IUnknown* pDevice, DXGI_SWAP_CHAIN_DESC* pDesc, IDXGISwapChain** ppSwapChain);
inline Hook<decltype(&CreateSwapChain)> hkCreateSwapChain = {};