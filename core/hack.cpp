#include "hack.h"
#include "../sdk/util/defines.h"

#include "interfaces.h"
#include "hooks/Present.h"
#include "hooks/ResizeBuffers.h"
#include "hooks/CreateSwapChain.h"

#include <d3d11.h>
#include <cassert>

#include <valve\ISwapChainDX11.h>

#include "../menu/menu.h"

void Hack::CreateRenderTarget() {
	if (FAILED(pSwapChain->pDXGISwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&pDevice)))
	{
		assert(false);
	}
	else
		pDevice->GetImmediateContext(&pContext);

	static const auto GetCorrectDXGIFormat = [](DXGI_FORMAT eCurrentFormat)
		{
			switch (eCurrentFormat)
			{
			case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
				return DXGI_FORMAT_R8G8B8A8_UNORM;
			}

			return eCurrentFormat;
		};

	DXGI_SWAP_CHAIN_DESC sd;
	pSwapChain->pDXGISwapChain->GetDesc(&sd);

	ID3D11Texture2D* pBackBuffer = nullptr;
	if (SUCCEEDED(pSwapChain->pDXGISwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer))))
	{
		if (pBackBuffer)
		{
			D3D11_RENDER_TARGET_VIEW_DESC desc{};
			desc.Format = static_cast<DXGI_FORMAT>(GetCorrectDXGIFormat(sd.BufferDesc.Format));
			desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			if (FAILED(pDevice->CreateRenderTargetView(pBackBuffer, &desc, &pRTV)))
			{
				desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
				if (FAILED(pDevice->CreateRenderTargetView(pBackBuffer, &desc, &pRTV)))
				{
					if (FAILED(pDevice->CreateRenderTargetView(pBackBuffer, NULL, &pRTV)))
					{
						assert(false);
					}
				}
			}
			pBackBuffer->Release();
			pBackBuffer = nullptr;
		}
	}
}

void Hack::DestroyRenderTarget() {
	if (pRTV != nullptr) {
		pRTV->Release();
		pRTV = nullptr;
	}
}

void Hack::init() {
	{
		gInterfaces.addModule(X("tier0.dll"));
		gInterfaces.addModule(X("schemasystem.dll"));
	}
	gInterfaces.init();

	gInterfaces.findInterface(HASH("SchemaSystem_00"));

	pSwapChain = **reinterpret_cast<ISwapChainDx11***>(gMemory.ResolveRelativeAddress(gMemory.PatternScan(X("rendersystemdx11.dll"), X("48 89 2D ? ? ? ? 48 C7 05 ? ? ? ? ? ? ? ? C7 05 ? ? ? ? ? ? ? ? 89 2D")), 0x3, 0x7));
	
	if (pSwapChain != nullptr)
	{
		if (FAILED(pSwapChain->pDXGISwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&pDevice)))
		{
			assert(false);
			return;
		}
		else
			pDevice->GetImmediateContext(&pContext);
	}

	MH_Initialize();
	{	
		hkPresent.detour(gMemory.get(pSwapChain->pDXGISwapChain, 8), Present);
		hkResizeBuffers.detour(gMemory.get(pSwapChain->pDXGISwapChain, 13), ResizeBuffers);

		IDXGIDevice* pDXGIDevice = NULL;
		pDevice->QueryInterface(IID_PPV_ARGS(&pDXGIDevice));

		IDXGIAdapter* pDXGIAdapter = NULL;
		pDXGIDevice->GetAdapter(&pDXGIAdapter);

		IDXGIFactory* pIDXGIFactory = NULL;
		pDXGIAdapter->GetParent(IID_PPV_ARGS(&pIDXGIFactory));

		hkCreateSwapChain.detour(gMemory.get(pIDXGIFactory, 10), CreateSwapChain);

		pDXGIDevice->Release();
		pDXGIAdapter->Release();
		pIDXGIFactory->Release();
	}
}

void Hack::exit() {
	MH_RemoveHook(MH_ALL_HOOKS);
	MH_Uninitialize();
	gMenu.exit();
	gInterfaces.exit();
}