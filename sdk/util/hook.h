#pragma once
#include <minhook\minhook.h>

template <typename T>
class Hook {
public:
	void* pOriginal;
	void* pTarget;
	void* pDetour;
	bool hooked;
public:
	bool detour(void* target, void* detour) {
		{
			if (!target || !detour) {
				return false;
			}

			this->pTarget = target;
			this->pDetour = detour;

			MH_STATUS status = MH_CreateHook(target, detour, &pOriginal);
			if (status != MH_OK) {
				return false;
			}
		}

		{
			if (hooked)
				return false;

			MH_STATUS status = MH_EnableHook(pTarget);
			if (status != MH_OK) {
				return false;
			}

			hooked = true;
		}

		return true;
	}

	bool restore() {
		if (!hooked)
			return false;

		MH_STATUS status = MH_DisableHook(pTarget);
		if (status != MH_OK) {
			return false;
		}

		hooked = false;
		return true;
	}

	T original() {
		return reinterpret_cast<T>(pOriginal);
	}
};