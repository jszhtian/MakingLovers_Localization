// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "compact_enc_det/compact_enc_det.h"
typedef LCID (WINAPI* fnGetUserDefaultLCID)(void);
typedef LANGID (WINAPI* fnGetSystemDefaultLangID)(void);
typedef LANGID (WINAPI* fnGetSystemDefaultUILanguage)(void);
typedef bool (WINAPI* fnSetWindowTextA)(HWND hWnd, LPCSTR lpString);
typedef bool (WINAPI* fnTextOutA)(HDC hdc, int nXStart, int nYStart, LPCSTR lpString, int cchString);
typedef HFONT(WINAPI* fnCreateFontA)(_In_ int     nHeight,
	_In_ int     nWidth,
	_In_ int     nEscapement,
	_In_ int     nOrientation,
	_In_ int     fnWeight,
	_In_ DWORD   fdwItalic,
	_In_ DWORD   fdwUnderline,
	_In_ DWORD   fdwStrikeOut,
	_In_ DWORD   fdwCharSet,
	_In_ DWORD   fdwOutputPrecision,
	_In_ DWORD   fdwClipPrecision,
	_In_ DWORD   fdwQuality,
	_In_ DWORD   fdwPitchAndFamily,
	_In_ LPCTSTR lpszFace);
typedef int (WINAPI* fnEnumFontFamiliesExA)(
	HDC           hdc,
	LPLOGFONTA    lpLogfont,
	FONTENUMPROCA lpProc,
	LPARAM        lParam,
	DWORD         dwFlags
	);
fnCreateFontA pCreateFontA;
fnGetSystemDefaultLangID pGetSystemDefaultLangID;
fnGetSystemDefaultUILanguage pGetSystemDefaultUILanguage;
fnGetUserDefaultLCID pGetUserDefaultLCID;
fnSetWindowTextA pSetWindowTextA;
fnTextOutA pTextOutA;
fnEnumFontFamiliesExA pEnumFontFamiliesExA;
wchar_t* ctow(const char *str)
{
	wchar_t* buffer=nullptr;
	if (str)
	{
		size_t nu = strlen(str);
		size_t n = (size_t)MultiByteToWideChar(932, 0, (const char *)str, int(nu), NULL, 0);
		buffer = 0;
		buffer = new wchar_t[n];
		memset(buffer, 0, sizeof(wchar_t)*(n));
		MultiByteToWideChar(932, 0, (const char *)str, int(nu), buffer, int(n));
	}
	return buffer;
}

wchar_t* ctow936(const char *str)
{
	wchar_t* buffer = nullptr;
	if (str)
	{
		size_t nu = strlen(str);
		size_t n = (size_t)MultiByteToWideChar(936, 0, (const char *)str, int(nu), NULL, 0);
		buffer = 0;
		buffer = new wchar_t[n];
		memset(buffer, 0, sizeof(wchar_t)*(n));
		MultiByteToWideChar(936, 0, (const char *)str, int(nu), buffer, int(n));
	}
	return buffer;
}

HFONT WINAPI newCreateFontA(_In_ int     nHeight,
	_In_ int     nWidth,
	_In_ int     nEscapement,
	_In_ int     nOrientation,
	_In_ int     fnWeight,
	_In_ DWORD   fdwItalic,
	_In_ DWORD   fdwUnderline,
	_In_ DWORD   fdwStrikeOut,
	_In_ DWORD   fdwCharSet,
	_In_ DWORD   fdwOutputPrecision,
	_In_ DWORD   fdwClipPrecision,
	_In_ DWORD   fdwQuality,
	_In_ DWORD   fdwPitchAndFamily,
	_In_ LPCSTR lpszFace)
{
	//char newlpszF[] = "宋体";
	//wchar_t* newlpszF = ctow936((char*)lpszFace);


	wchar_t* wcs = nullptr;
	fdwCharSet = GB2312_CHARSET;
	char buffer[0x100] = { 0 };
	memset(buffer, 0, 0x100);
	memcpy(buffer, lpszFace, strlen(lpszFace));
	wcs = ctow936(buffer);
	return CreateFontW(nHeight, nWidth, nEscapement, nOrientation, fnWeight, fdwItalic, fdwUnderline, fdwStrikeOut, fdwCharSet, fdwOutputPrecision, fdwClipPrecision, fdwQuality, fdwPitchAndFamily, wcs);
}

LCID WINAPI newGetUserDefaultLCID()
{
	return 0x411;
}

LANGID WINAPI newGetSystemDefaultLangID()
{
	return 0x411;
}

LANGID WINAPI newGetSystemDefaultUILanguage()
{
	return 0x411;
}

bool WINAPI newSetWindowTextA(HWND hw, LPCSTR lps)
{
	wchar_t chk[] = L"Making";
	wchar_t* oldTitle = ctow(lps);
	//wchar_t* newtitle = ctow(lps);
	if (memcmp(chk, oldTitle, 6) == 0)
	{
		wchar_t newtitle[] = L"【潮鸣市电视台咲姐姐后援会】Making Lovers 简体中文版 ver1.0";
		return SetWindowTextW(hw, newtitle);
	}
	else
	{
		return ((fnSetWindowTextA)pSetWindowTextA)(hw, lps);
	}
}

int WINAPI newEnumFontFamiliesExA(HDC hdc, LPLOGFONTA lpLogfont, FONTENUMPROCA lpProc, LPARAM lParam, DWORD dwFlags)
{
	//lpLogfont->lfCharSet = 936;
	LOGFONTA Lfont;
	memcpy(&Lfont, lpLogfont, sizeof(LOGFONTA));
	Lfont.lfCharSet = GB2312_CHARSET;
	memcpy(lpLogfont, &Lfont, sizeof(LOGFONTA));
	return ((fnEnumFontFamiliesExA)pEnumFontFamiliesExA)(hdc, lpLogfont, lpProc, lParam, dwFlags);
}

/*
bool WINAPI newTextOutA(HDC hw, int x, int y, LPCSTR lpStr, int len)
{
	wchar_t* wcs = nullptr;
	char buffer[0x100] = { 0 };
	memset(buffer, 0, 0x100);
	memcpy(buffer, lpStr, len);
	bool is_reliable;
	int bytes_consumed; 
	Encoding encoding = CompactEncDet::DetectEncoding(buffer, len, nullptr, nullptr, nullptr, UNKNOWN_ENCODING, UNKNOWN_LANGUAGE, CompactEncDet::WEB_CORPUS, true, &bytes_consumed, &is_reliable);
	if (encoding == JAPANESE_SHIFT_JIS || encoding == JAPANESE_CP932 && is_reliable)
	{
		wcs = ctow(buffer);
		return TextOutW(hw, x, y, wcs, len/2);
	}
	else
	{
		//wcs = ctow936(lpStr);
		return ((fnTextOutA)pTextOutA)(hw, x, y, lpStr, len);
	}
}
*/
void SetHook()
{
	pCreateFontA = (fnCreateFontA)GetProcAddress(GetModuleHandle(L"gdi32.dll"), "CreateFontA");
	pEnumFontFamiliesExA = (fnEnumFontFamiliesExA)GetProcAddress(GetModuleHandle(L"gdi32.dll"), "EnumFontFamiliesExA");
	pGetSystemDefaultLangID = (fnGetSystemDefaultLangID)GetProcAddress(GetModuleHandle(L"Kernel32.dll"), "GetSystemDefaultLangID");
	pGetSystemDefaultUILanguage=(fnGetSystemDefaultUILanguage)GetProcAddress(GetModuleHandle(L"Kernel32.dll"), "GetSystemDefaultUILanguage");
	pGetUserDefaultLCID=(fnGetUserDefaultLCID)GetProcAddress(GetModuleHandle(L"Kernel32.dll"), "GetUserDefaultLCID");
	pSetWindowTextA=(fnSetWindowTextA)GetProcAddress(GetModuleHandle(L"User32.dll"), "SetWindowTextA");
	DetourTransactionBegin();
	DetourAttach((void**)&pCreateFontA, newCreateFontA);
	DetourAttach((void**)&pGetSystemDefaultLangID, newGetSystemDefaultLangID);
	DetourAttach((void**)&pGetSystemDefaultUILanguage, newGetSystemDefaultUILanguage);
	DetourAttach((void**)&pGetUserDefaultLCID, newGetUserDefaultLCID);
	DetourAttach((void**)&pSetWindowTextA, newSetWindowTextA);
	DetourAttach((void**)&pEnumFontFamiliesExA, newEnumFontFamiliesExA);
	if (DetourTransactionCommit() != NO_ERROR)
	{
		MessageBoxW(NULL, L"Hook目标函数失败", L"严重错误", MB_OK | MB_ICONWARNING);
	}
}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		SetHook();
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

extern "C" __declspec(dllexport) void dummy(void) {
	return;
}