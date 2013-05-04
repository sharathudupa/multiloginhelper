// IETabProcControl.cpp : Implementation of CIETabProcControl
#include "stdafx.h"
#include "IETabProcControl.h"
#include <exdisp.h>
#include <exdispid.h>
#include <shlguid.h>
#include <shlwapi.h>
#include <strsafe.h>

// CIETabProcControl
HWND g_hwndApp = NULL; 
IOleInPlaceActiveObject* g_pIOleInPlaceActiveObject = NULL;

HWND CIETabProcControl::CreateControlWindow(HWND hwndParent, RECT &rcPos)
{
    CAxWindow axWin;
    HWND hwndCtrl;
    HWND hwndIE;

    hwndCtrl = Create(hwndParent, rcPos, NULL, WS_CHILD|WS_VISIBLE);

    hwndIE = axWin.Create(hwndCtrl, rcPos, L"{8856F961-340A-11D0-A96B-00C04FD705A2}", WS_CHILD|WS_VISIBLE);
    ::SetWindowPos(hwndIE, hwndCtrl, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

    g_hwndApp = hwndCtrl;

    axWin.QueryControl(IID_IWebBrowser2, (void**)&m_spInnerWebBrowser);
    if (m_spInnerWebBrowser)
    {
        WCHAR wszUrl[2048] = {0};

        m_spInnerWebBrowser->QueryInterface(IID_IOleInPlaceActiveObject, 
                                            (void**)&g_pIOleInPlaceActiveObject);

        CComPtr<IConnectionPointContainer> spConnectionPointContainer;
        m_spInnerWebBrowser->QueryInterface(IID_IConnectionPointContainer,(void**)&spConnectionPointContainer);
        if (spConnectionPointContainer)
        {
            spConnectionPointContainer->FindConnectionPoint(DIID_DWebBrowserEvents2,&m_spCP);
            if (m_spCP)
            {
                m_spCP->Advise((IDispatch*)this,&m_dwCookie);
            }
        }

        if (ExpandEnvironmentStrings(L"%programfiles%\\multiloginhelper\\help.htm", 
                                     wszUrl, 2048))
        {
            m_spInnerWebBrowser->Navigate(wszUrl, NULL, NULL, NULL, NULL);
        }
        else
        {
            m_spInnerWebBrowser->Navigate(L"www.live.com", NULL, NULL, NULL, NULL);
        }
    }

    return hwndCtrl;
}

HRESULT CIETabProcControl::SetClientSite(IOleClientSite* pClientSite)
{
    if (pClientSite)
    {
        CComPtr<IServiceProvider> spIsp, spIsp2;

		pClientSite->QueryInterface(IID_IServiceProvider, (void **)&spIsp);

        if (spIsp)
        {
		    spIsp->QueryService(SID_STopLevelBrowser, IID_IServiceProvider, (void **)&spIsp2);

            if (spIsp2)
            {
		        spIsp2->QueryService(SID_SWebBrowserApp, IID_IWebBrowser2, (void **)&m_spWebBrowser);
            }
        }
    }

	return IOleObject_SetClientSite(pClientSite);
}

HRESULT CIETabProcControl::Done()
{
    m_spCP->Unadvise(m_dwCookie);
    return S_OK;
}

HRESULT CIETabProcControl::Navigate(BSTR bstrUrl)
{
    if (m_spInnerWebBrowser)
    {
        m_spInnerWebBrowser->Navigate(bstrUrl, NULL, NULL, NULL, NULL);
    }

    return S_OK;
}

HRESULT CIETabProcControl::Invoke(DISPID dispidMember, const IID &riid, LCID lcid, WORD wFlags, DISPPARAMS *pdispparams, VARIANT *pvarResult, EXCEPINFO *pexecpinfo, UINT *puArgErr)
{
	HRESULT hr = S_OK;
	
	switch(dispidMember)
	{
        case DISPID_TITLECHANGE:
            {
                CComBSTR bstrTitle(pdispparams->rgvarg[0].bstrVal);
                if (NULL != bstrTitle && NULL != m_spWebBrowser)
                {
                    CComQIPtr<IHTMLDocument2> spDoc2;
                    CComPtr<IDispatch> spDisp;
                    m_spWebBrowser->get_Document(&spDisp);
                    if (spDisp && NULL != (spDoc2 = spDisp))
                    {
                        spDoc2->put_title(bstrTitle);
                    }
                }
            }
            break;

		case DISPID_ONQUIT:
        	hr = m_spCP->Unadvise(m_dwCookie);
			break;

		default:
            hr = __super::Invoke(dispidMember, riid, lcid, wFlags, pdispparams, pvarResult, pexecpinfo, puArgErr);
			break;
	}
	
	return hr;
}
