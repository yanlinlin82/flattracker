// DllMain.cpp

#include "StdAfx.h"
#include "Resource.h"
#include "FlatTracker_i.h"

///////////////////////////////////////////////////////////////////////////

class CFlatTrackerModule : public CAtlDllModuleT<CFlatTrackerModule>
{
public :
	DECLARE_LIBID(LIBID_FlatTrackerLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_FLATTRACKER, "{D89F03C1-81B8-46A6-972F-441B45E34EEA}")
};

CFlatTrackerModule _AtlModule;

///////////////////////////////////////////////////////////////////////////

STDAPI DllCanUnloadNow()
{
	return _AtlModule.DllCanUnloadNow();
}

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
	return _AtlModule.DllGetClassObject(rclsid, riid, ppv);
}

STDAPI DllRegisterServer()
{
	HRESULT hr = _AtlModule.DllRegisterServer();
	return hr;
}

STDAPI DllUnregisterServer()
{
	HRESULT hr = _AtlModule.DllUnregisterServer();
	return hr;
}

STDAPI DllInstall(BOOL bInstall, LPCWSTR pszCmdLine)
{
	HRESULT hr = E_FAIL;
	static const wchar_t szUserSwitch[] = L"user";

	if (pszCmdLine != NULL)
	{
		if (_wcsnicmp(pszCmdLine, szUserSwitch, _countof(szUserSwitch)) == 0)
		{
			AtlSetPerUserRegistration(true);
		}
	}

	if (bInstall)
	{	
		hr = DllRegisterServer();
		if (FAILED(hr))
		{	
			DllUnregisterServer();
		}
	}
	else
	{
		hr = DllUnregisterServer();
	}
	return hr;
}

///////////////////////////////////////////////////////////////////////////

HINSTANCE g_hDll = NULL;

BOOL WINAPI DllMain(HINSTANCE hDll, DWORD reason, LPVOID reserved)
{
#ifndef NDEBUG
	if (reason == DLL_PROCESS_ATTACH)
	{
		::AllocConsole();
		FILE* fp = NULL;
		freopen_s(&fp, "CON", "w", stdout);
		freopen_s(&fp, "CON", "w", stderr);
	}
	else if (reason == DLL_PROCESS_DETACH)
	{
		::FreeConsole();
	}
#endif
	if (reason == DLL_PROCESS_ATTACH)
	{
		g_hDll = hDll;
	}
	return _AtlModule.DllMain(reason, reserved);
}

///////////////////////////////////////////////////////////////////////////
