#include "NxCore/Platform.h"
#include "NxCore/StringUtils.h"
#include "NxCore/Logger.h"

#include <Windows.h>
#include <commdlg.h>
#include <ShlObj_core.h>
#include <codecvt>

namespace Nexus::Platform::Utils
{
	static std::string GetLastErrorMessage()
	{
		auto errorMessageID = GetLastError();

		LPSTR messageBuffer = nullptr;
		size_t size = FormatMessageA(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			errorMessageID,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPSTR)&messageBuffer,
			0,
			NULL);

		std::string message(messageBuffer, size);
		LocalFree(messageBuffer);

		return message;
	}
}

std::string Nexus::Platform::OpenFile(const char* Filter)
{
	OPENFILENAMEA ofn;

	CHAR szFile[260] = { 0 };

	ZeroMemory(&ofn, sizeof(OPENFILENAME));

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = nullptr;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = Filter;
	ofn.nFilterIndex = 1;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

	if (GetOpenFileNameA(&ofn) == TRUE)
	{
		return ofn.lpstrFile;
	}

	return std::string();
}

std::string Nexus::Platform::SaveFile(const char* Filter)
{
	OPENFILENAMEA ofn;

	CHAR szFile[260] = { 0 };

	ZeroMemory(&ofn, sizeof(OPENFILENAME));

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = nullptr;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = Filter;
	ofn.nFilterIndex = 1;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

	if (GetSaveFileNameA(&ofn) == TRUE)
	{
		return ofn.lpstrFile;
	}

	return std::string();
}

std::string Nexus::Platform::SelectFolder()
{
	TCHAR szDir[260] = { 0 };

	BROWSEINFO bInfo;
	bInfo.hwndOwner = nullptr;
	bInfo.pidlRoot = NULL;
	bInfo.pszDisplayName = szDir;
	bInfo.lpszTitle = L"Select a folder";
	bInfo.ulFlags = 0;
	bInfo.lpfn = NULL;
	bInfo.lParam = 0;
	bInfo.iImage = -1;

	LPITEMIDLIST lpItem = SHBrowseForFolder(&bInfo);
	if (lpItem != NULL)
	{
		SHGetPathFromIDList(lpItem, szDir);

		int wideStrLength = static_cast<int>(wcslen(bInfo.pszDisplayName));
		int bufferSize = WideCharToMultiByte(CP_UTF8, 0, bInfo.pszDisplayName, wideStrLength, nullptr, 0, nullptr, nullptr);

		std::string result(bufferSize, '\0');
		WideCharToMultiByte(CP_UTF8, 0, bInfo.pszDisplayName, wideStrLength, &result[0], bufferSize, nullptr, nullptr);

		return result;
	}

	return std::string();
}

struct Nexus::Platform::LibraryHANDLE
{
	HMODULE Module;
};

Nexus::Platform::LibraryHANDLE* Nexus::Platform::LoadSharedLibrary(const std::string& path)
{
	auto wstr = string_to_wstring(path);
	auto handle = LoadLibrary(wstr.c_str());

	if (handle == NULL)
	{
		NEXUS_LOG("Platform", "Failed to load Library: %s", path.c_str());
		return nullptr;
	}

	NEXUS_LOG("Platform", "Loaded Library: %s", path.c_str());
	
	LibraryHANDLE* lib = new LibraryHANDLE;
	lib->Module = handle;

	return lib;
}

void Nexus::Platform::UnloadSharedLibrary(LibraryHANDLE* pHandle, const std::string& path)
{
	auto handle = pHandle->Module;
	if (!FreeLibrary(handle))
	{
		auto error = Utils::GetLastErrorMessage();
		NEXUS_LOG("Platform", "Failed to unload Library: %s | Error - %s", path.c_str(), error.c_str());
	}
	else
	{
		NEXUS_LOG("Platform", "Unloaded Library: %s ", path.c_str());
	}
		
	delete pHandle;
	pHandle = nullptr;
}

void* Nexus::Platform::LoadFuncFromSharedLibrary(LibraryHANDLE* pHandle, const char* funcName)
{
	auto func = GetProcAddress(pHandle->Module, funcName);
	if (!func)
	{
		auto error = Utils::GetLastErrorMessage();
		NEXUS_LOG("Platform", "Unable to load Func: %s | Error - %s", funcName, error.c_str());
	}

	return func;
}
