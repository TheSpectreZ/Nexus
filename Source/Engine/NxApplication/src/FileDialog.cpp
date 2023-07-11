#include "NxApplication/FileDialog.h"
#include <Windows.h>
#include <commdlg.h>
#include <ShlObj_core.h>
#include <codecvt>

namespace Nexus::FileDialog
{
	HWND m_window = nullptr;
}

void Nexus::FileDialog::SetContextWindow(const Window& Window)
{
	m_window = (HWND)Window.nativeHandle;
}

std::string Nexus::FileDialog::OpenFile(const char* Filter)
{
	OPENFILENAMEA ofn;

	CHAR szFile[260] = { 0 };

	ZeroMemory(&ofn, sizeof(OPENFILENAME));

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = m_window;
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

std::string Nexus::FileDialog::SaveFile(const char* Filter)
{
	OPENFILENAMEA ofn;

	CHAR szFile[260] = { 0 };

	ZeroMemory(&ofn, sizeof(OPENFILENAME));

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = m_window;
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

std::string Nexus::FileDialog::SelectFolder()
{
	TCHAR szDir[260] = { 0 };

	BROWSEINFO bInfo;
	bInfo.hwndOwner = m_window;
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