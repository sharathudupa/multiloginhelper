// TabProc.cpp : Implementation of WinMain


#include "stdafx.h"
#include "resource.h"
#include "TabProc.h"
#include "strsafe.h"

#define MLH_GUID_KEY L"{0E4A5D4F-ECE4-4bcf-BB2E-DD483EC68D24}"

extern IOleInPlaceActiveObject* g_pIOleInPlaceActiveObject;

bool IsVistaOS()
{
    static bool fRet = false;
    static bool fInitialized = false;

    if (!fInitialized)
    {
        OSVERSIONINFO osvi = {0};

        osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
        if (GetVersionEx(&osvi))
        {
            fRet = (osvi.dwMajorVersion >= 6);
            fInitialized = true;
        }
    }

    return fRet;
}

void UpdateElevationPolicy(bool bRegister)
{
    DWORD dwType = REG_DWORD;
    DWORD dwValue = 0;
    DWORD dwSize = sizeof(DWORD);
    HKEY hk;

    if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"Software\\Microsoft\\Internet Explorer\\Low Rights\\ElevationPolicy",
                                    0, KEY_WRITE, &hk))
    {
        HKEY hkPol;
        DWORD dwDisp;
        if (bRegister)
        {
            if (ERROR_SUCCESS == RegCreateKeyEx(hk, MLH_GUID_KEY, 0, NULL, 0, KEY_SET_VALUE, NULL, &hkPol, &dwDisp))
            {
                WCHAR wszPath[2048] = {0};
                if (GetModuleFileName(NULL, wszPath, 2048))
                {
                    DWORD dwPol = 1;
                    LPWSTR pwszFileName = PathFindFileName(wszPath);
                    RegSetValueEx(hkPol, L"AppName", 0, REG_SZ, (BYTE*)pwszFileName, (DWORD) (wcslen(pwszFileName) * sizeof(WCHAR)));

                    PathRemoveFileSpec(wszPath);
                    RegSetValueEx(hkPol, L"AppPath", 0, REG_SZ, (BYTE*)wszPath, (DWORD) (wcslen(wszPath) * (DWORD) sizeof(WCHAR)));

                    RegSetValueEx(hkPol, L"Policy", 0, REG_DWORD, (BYTE*)&dwPol, (DWORD) sizeof(DWORD));
                }

                RegCloseKey(hkPol);

                // refresh policy
                {
                    HMODULE hDll = LoadLibrary(L"ieframe.dll");
                    typedef HRESULT (*PFNREFRESH)();
                    PFNREFRESH pfnRefresh;
                    if (hDll)
                    {
                        pfnRefresh = (PFNREFRESH) GetProcAddress(hDll, "IERefreshElevationPolicy");
                        if (pfnRefresh)
                        {
                            pfnRefresh();
                        }

                        FreeLibrary(hDll);
                    }
                }
            }
        }
        else
        {
            RegDeleteKey(hk, MLH_GUID_KEY);
        }

        RegCloseKey(hk);
    }
}

class CTabProcModule : public CAtlExeModuleT< CTabProcModule >
{
public :
	DECLARE_LIBID(LIBID_TabProcLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_TABPROC, "{249AC468-8F7F-4E68-BD70-1437B5DCA60A}")

    bool ParseCommandLine(LPCTSTR lpCmdLine, HRESULT* pnRetCode)
    {
        if (IsVistaOS())
        {
		    WCHAR szTokens[] = _T("-/");

            LPCTSTR lpszToken = FindOneOf(lpCmdLine, szTokens);
		    while (lpszToken != NULL)
		    {
			    if (WordCmpI(lpszToken, _T("UnregServer"))==0)
			    {
                    UpdateElevationPolicy(false);
			    }
			    else if (WordCmpI(lpszToken, _T("RegServer"))==0)
			    {
                    UpdateElevationPolicy(true);
			    }

			    lpszToken = FindOneOf(lpszToken, szTokens);
		    }
        }

        return __super::ParseCommandLine(lpCmdLine, pnRetCode);
    }

	// extend the registration to ensure single use
	HRESULT RegisterClassObjects(DWORD dwClsContext, DWORD dwFlags) throw() 
	{ 
		dwFlags = ((dwFlags & ~(REGCLS_MULTIPLEUSE | REGCLS_MULTI_SEPARATE )) | REGCLS_SINGLEUSE); 
		return CAtlExeModuleT<CTabProcModule>::RegisterClassObjects(dwClsContext, dwFlags); 
	} 

    // extend the RunMessageLoop to process the tab key
	void RunMessageLoop()
	{
		MSG msg;
		while (GetMessage(&msg, 0, 0, 0) > 0)
		{
			TranslateMessage(&msg);
            
            if (g_pIOleInPlaceActiveObject)
            {
                if (S_OK == g_pIOleInPlaceActiveObject->TranslateAccelerator(&msg))
				{
					continue;
				}
            }

			DispatchMessage(&msg);
		}
	}
};

CTabProcModule _AtlModule;



//
extern "C" int WINAPI _tWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, 
                                LPTSTR /*lpCmdLine*/, int nShowCmd)
{
    return _AtlModule.WinMain(nShowCmd);
}

