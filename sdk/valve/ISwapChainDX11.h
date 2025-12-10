#pragma once
#include "../util/defines.h"

class IDXGISwapChain;
class ISwapChainDx11
{
	MEM_PAD(0x170);
	IDXGISwapChain* pDXGISwapChain;
};