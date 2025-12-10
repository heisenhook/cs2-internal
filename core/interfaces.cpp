#include "interfaces.h"
#include "../sdk/util/defines.h"

static const CInterfaceRegister* GetRegisterList(void* pModuleBase)
{
	if (!pModuleBase)
		return nullptr;

	std::uint8_t* pCreateInterface = reinterpret_cast<std::uint8_t*>(gMemory.GetModuleExport(pModuleBase, X("CreateInterface")));

	if (pCreateInterface == nullptr)
	{
		return nullptr;
	}

	return *reinterpret_cast<CInterfaceRegister**>(gMemory.ResolveRelativeAddress(pCreateInterface, 0x3, 0x7));
}

void Interfaces::init() {
	// loop through listed modules and
	// copy each interface to array
	for (auto& m : vModules) {
		auto registerList = GetRegisterList(m->pAddress);
		
		const CInterfaceRegister* pRegister = registerList;
		while (pRegister != nullptr) {
			void* pInterface = pRegister->fnCreate();
			if (!pInterface)
				return;

			interfaceType* i = new interfaceType();
			i->uHash = hash::RunTime(pRegister->szName);
			i->pAddress = pInterface;
#ifdef _DEBUG
			i->pRegister = pRegister;
#endif
			m->vInterfaces.push_back(i);

			pRegister = pRegister->pNext;
		}
	}
}

void Interfaces::exit() {
	for (auto& m : vModules) {
		for (auto& i : m->vInterfaces) {
			free(i);
		}
		free(m);
	}
}