#pragma once
#include "util.h"
#include "pe64.h"

class Memory {
public:
	template <typename Return, typename ... Arguments>
	constexpr Return call(void* pVmt, const std::uint32_t uIndex, Arguments ... args) noexcept {
		using Function = Return(__thiscall*)(void*, decltype(args)...);
		return (*static_cast<Function**>(pVmt))[uIndex](pVmt, args...);
	}

	constexpr void* get(void* pVmt, const std::uint32_t uIndex) noexcept {
		return (*static_cast<void***>(pVmt))[uIndex];
	}

	void* GetModuleBase(std::string szModuleName) {
		const _PEB* pPEB = reinterpret_cast<_PEB*>(__readgsqword(0x60));

		if (szModuleName == "")
			return pPEB->ImageBaseAddress;

		void* pModuleBase = nullptr;
		for (LIST_ENTRY* pListEntry = pPEB->Ldr->InMemoryOrderModuleList.Flink; pListEntry != &pPEB->Ldr->InMemoryOrderModuleList; pListEntry = pListEntry->Flink)
		{
			const _LDR_DATA_TABLE_ENTRY* pEntry = CONTAINING_RECORD(pListEntry, _LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);

			if (pEntry->FullDllName.Buffer != nullptr && strcmp(szModuleName.c_str(), narrow(pEntry->BaseDllName.Buffer).c_str()) == 0)
			{
				pModuleBase = pEntry->DllBase;
				break;
			}
		}

		return pModuleBase;
	}

	void* GetModuleExport(void* pModuleBase, std::string szExportName) {
		const auto pBaseAddress = static_cast<const std::uint8_t*>(pModuleBase);

		const auto pIDH = static_cast<const IMAGE_DOS_HEADER*>(pModuleBase);
		if (pIDH->e_magic != IMAGE_DOS_SIGNATURE)
			return nullptr;

		const auto pINH = reinterpret_cast<const IMAGE_NT_HEADERS64*>(pBaseAddress + pIDH->e_lfanew);
		if (pINH->Signature != IMAGE_NT_SIGNATURE)
			return nullptr;

		const IMAGE_OPTIONAL_HEADER64* pIOH = &pINH->OptionalHeader;
		const std::uintptr_t nExportDirectorySize = pIOH->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size;
		const std::uintptr_t uExportDirectoryAddress = pIOH->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;

		if (nExportDirectorySize == 0U || uExportDirectoryAddress == 0U)
		{
			return nullptr;
		}

		const auto pIED = reinterpret_cast<const IMAGE_EXPORT_DIRECTORY*>(pBaseAddress + uExportDirectoryAddress);
		const auto pNamesRVA = reinterpret_cast<const std::uint32_t*>(pBaseAddress + pIED->AddressOfNames);
		const auto pNameOrdinalsRVA = reinterpret_cast<const std::uint16_t*>(pBaseAddress + pIED->AddressOfNameOrdinals);
		const auto pFunctionsRVA = reinterpret_cast<const std::uint32_t*>(pBaseAddress + pIED->AddressOfFunctions);

		// Perform binary search to find the export by name
		std::size_t nRight = pIED->NumberOfNames, nLeft = 0U;
		while (nRight != nLeft)
		{
			// Avoid INT_MAX/2 overflow
			const std::size_t uMiddle = nLeft + ((nRight - nLeft) >> 1U);
			const int iResult = strcmp(szExportName.c_str(), reinterpret_cast<const char*>(pBaseAddress + pNamesRVA[uMiddle]));

			if (iResult == 0)
			{
				const std::uint32_t uFunctionRVA = pFunctionsRVA[pNameOrdinalsRVA[uMiddle]];

							// Check if it's a forwarded export
				if (uFunctionRVA >= uExportDirectoryAddress && uFunctionRVA - uExportDirectoryAddress < nExportDirectorySize)
				{
					// Forwarded exports are not supported
					break;
				}

				return const_cast<std::uint8_t*>(pBaseAddress) + uFunctionRVA;
			}

			if (iResult > 0)
				nLeft = uMiddle + 1;
			else
				nRight = uMiddle;
		}

		// Export not found
		return nullptr;
	}

	// simple Pattern/AOB/Signature scanner
	std::uint8_t* PatternScan(std::string szModuleName, std::string szPattern) noexcept {
		static auto patternToByte = [](const char* pattern) noexcept -> const std::vector<std::int32_t>
			{
				std::vector<std::int32_t> bytes = std::vector<std::int32_t>{ };
				char* start = const_cast<char*>(pattern);
				const char* end = const_cast<char*>(pattern) + std::strlen(pattern);

				for (auto current = start; current < end; ++current)
				{
					if (*current == '?')
					{
						++current;

						if (*current == '?')
							++current;

						bytes.push_back(-1);
					}
					else
						bytes.push_back(std::strtoul(current, &current, 16));
				}

				return bytes;
			};

		void* handle = GetModuleBase(szModuleName);

		if (!handle)
			return nullptr;

		const PIMAGE_DOS_HEADER dosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(handle);
		const PIMAGE_NT_HEADERS ntHeaders =
			reinterpret_cast<PIMAGE_NT_HEADERS>(reinterpret_cast<std::uint8_t*>(handle) + dosHeader->e_lfanew);

		const std::uintptr_t size = ntHeaders->OptionalHeader.SizeOfImage;
		const std::vector<std::int32_t> bytes = patternToByte(szPattern.c_str());
		std::uint8_t* scanBytes = reinterpret_cast<std::uint8_t*>(handle);

		const std::size_t s = bytes.size();
		const std::int32_t* d = bytes.data();

		for (std::size_t i = 0ul; i < size - s; ++i)
		{

			bool found = true;

			for (std::size_t j = 0ul; j < s; ++j)
			{
				if (scanBytes[i + j] != d[j] && d[j] != -1)
				{
					found = false;
					break;
				}
			}

			if (found)
				return &scanBytes[i];
		}
		return nullptr;
	}

	__forceinline std::uint8_t* ResolveRelativeAddress(std::uint8_t* nAddressBytes, std::uint32_t nRVAOffset, std::uint32_t nRIPOffset)
	{
		if (nAddressBytes)
		{
			std::uint32_t nRVA = *reinterpret_cast<std::uint32_t*>(nAddressBytes + nRVAOffset);
			std::uint64_t nRIP = reinterpret_cast<std::uint64_t>(nAddressBytes) + nRIPOffset;

			return reinterpret_cast<std::uint8_t*>(nRVA + nRIP);
		}
		else
		{
			return nullptr;
		}
	}

}; inline Memory gMemory;