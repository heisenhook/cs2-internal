#pragma once

class ISwapChainDx11;
class ID3D11Device;
class ID3D11DeviceContext;
class ID3D11RenderTargetView;

class Hack {
public:
	bool bRunning = true;
public:
	ISwapChainDx11* pSwapChain = nullptr;
	ID3D11Device* pDevice = nullptr;
	ID3D11DeviceContext* pContext = nullptr;
	ID3D11RenderTargetView* pRTV = nullptr;
public:
	void CreateRenderTarget();
	void DestroyRenderTarget();

	void init();
	void exit();
}; inline Hack gHack;

// Example hook definition
// extern HRESULT __stdcall Present(IDirect3DDevice9* pDevice, const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion);
// inline Hook<decltype(&Present)> hkPresent = {};
//
// Example hook usage
// hkPresent.detour(gMemory.get_method(device, 17), Present);