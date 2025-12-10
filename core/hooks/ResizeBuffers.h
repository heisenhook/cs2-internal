#pragma once
#include <util\hook.h>
#include <cstdint>
#include <d3d11.h>

extern HRESULT __fastcall ResizeBuffers(IDXGISwapChain* pSwapChain, std::uint32_t nBufferCount, std::uint32_t nWidth, std::uint32_t nHeight, DXGI_FORMAT newFormat, std::uint32_t nFlags);
inline Hook<decltype(&ResizeBuffers)> hkResizeBuffers = {};