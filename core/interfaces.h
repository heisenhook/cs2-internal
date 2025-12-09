#pragma once
#include <string>
#include <vector>
#include <cstdint>

#include "../sdk/util/hash.h"
#include "../sdk/util/memory.h"

using InstantiateInterfaceFn_t = void* (*)();
class CInterfaceRegister
{
public:
	InstantiateInterfaceFn_t fnCreate;
	const char* szName;
	CInterfaceRegister* pNext;
};


struct interfaceType {
	std::uint32_t uHash;
	void* pAddress;
#ifdef _DEBUG
	const CInterfaceRegister* pRegister;
#endif // _DEBUG
};

struct moduleType {
	std::uint32_t uHash;
	void* pAddress;
	std::vector<interfaceType*> vInterfaces;
};

class Interfaces {
public:
	std::vector<moduleType*> vModules;

	void addModule(std::string szModuleName) {
		const auto hash = hash::RunTime(szModuleName.c_str());
		for (auto& m : vModules) {
			if (m->uHash == hash)
				return;
		}

		moduleType* m = new moduleType();
		m->uHash = hash;
		m->pAddress = gMemory.GetModuleBase(szModuleName);
		m->vInterfaces = {};
		vModules.push_back(m);
	}

	template <typename T = void*>
	T findInterface(std::uint32_t uInterfaceHash) {
		for (auto& m : vModules) {
			for (auto& i : m->vInterfaces) {
				if (i->uHash == uInterfaceHash)
					return static_cast<T>(i->pAddress);
			}
		}
		return nullptr;
	}
public:
	void init();
	void exit();
}; inline Interfaces gInterfaces;