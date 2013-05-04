// IETabProcControl.h : Declaration of the CIETabProcControl
#pragma once
#include "resource.h"       // main symbols
#include <atlctl.h>
#include "TabProc.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

extern IOleInPlaceActiveObject* g_pIOleInPlaceActiveObject;

// CIETabProcControl
class ATL_NO_VTABLE CIETabProcControl :
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IIETabProcControl, &IID_IIETabProcControl, &LIBID_TabProcLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IPersistStreamInitImpl<CIETabProcControl>,
	public IOleControlImpl<CIETabProcControl>,
	public IOleObjectImpl<CIETabProcControl>,
	public IOleInPlaceActiveObjectImpl<CIETabProcControl>,
	public IViewObjectExImpl<CIETabProcControl>,
	public IOleInPlaceObjectWindowlessImpl<CIETabProcControl>,
	public ISupportErrorInfo,
	public IObjectWithSiteImpl<CIETabProcControl>,
	public IPersistStorageImpl<CIETabProcControl>,
	public ISpecifyPropertyPagesImpl<CIETabProcControl>,
	public IQuickActivateImpl<CIETabProcControl>,
#ifndef _WIN32_WCE
	public IDataObjectImpl<CIETabProcControl>,
#endif
	public IProvideClassInfo2Impl<&CLSID_IETabProcControl, NULL, &LIBID_TabProcLib>,
	public IObjectSafetyImpl<CIETabProcControl, INTERFACESAFE_FOR_UNTRUSTED_CALLER>,
	public CComCoClass<CIETabProcControl, &CLSID_IETabProcControl>,
	public CComControl<CIETabProcControl>
{
public:


    CIETabProcControl()
	{
	}

DECLARE_OLEMISC_STATUS(OLEMISC_RECOMPOSEONRESIZE |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_INSIDEOUT |
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST
)

DECLARE_REGISTRY_RESOURCEID(IDR_IETABPROCCONTROL)


DECLARE_NOT_AGGREGATABLE(CIETabProcControl)

BEGIN_COM_MAP(CIETabProcControl)
	COM_INTERFACE_ENTRY(IIETabProcControl)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IViewObjectEx)
	COM_INTERFACE_ENTRY(IViewObject2)
	COM_INTERFACE_ENTRY(IViewObject)
	COM_INTERFACE_ENTRY(IOleInPlaceObjectWindowless)
	COM_INTERFACE_ENTRY(IOleInPlaceObject)
	COM_INTERFACE_ENTRY2(IOleWindow, IOleInPlaceObjectWindowless)
	COM_INTERFACE_ENTRY(IOleInPlaceActiveObject)
	COM_INTERFACE_ENTRY(IOleControl)
	COM_INTERFACE_ENTRY(IOleObject)
	COM_INTERFACE_ENTRY(IPersistStreamInit)
	COM_INTERFACE_ENTRY2(IPersist, IPersistStreamInit)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY(ISpecifyPropertyPages)
	COM_INTERFACE_ENTRY(IQuickActivate)
	COM_INTERFACE_ENTRY(IPersistStorage)
#ifndef _WIN32_WCE
	COM_INTERFACE_ENTRY(IDataObject)
#endif
	COM_INTERFACE_ENTRY(IProvideClassInfo)
	COM_INTERFACE_ENTRY(IProvideClassInfo2)
	COM_INTERFACE_ENTRY(IObjectWithSite)
	COM_INTERFACE_ENTRY_IID(IID_IObjectSafety, IObjectSafety)
END_COM_MAP()

BEGIN_PROP_MAP(CIETabProcControl)
	PROP_DATA_ENTRY("_cx", m_sizeExtent.cx, VT_UI4)
	PROP_DATA_ENTRY("_cy", m_sizeExtent.cy, VT_UI4)
	// Example entries
	// PROP_ENTRY("Property Description", dispid, clsid)
	// PROP_PAGE(CLSID_StockColorPage)
END_PROP_MAP()


BEGIN_MSG_MAP(CIETabProcControl)
	CHAIN_MSG_MAP(CComControl<CIETabProcControl>)
	DEFAULT_REFLECTION_HANDLER()
END_MSG_MAP()
// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid)
	{
		static const IID* arr[] =
		{
			&IID_IIETabProcControl,
		};

		for (int i=0; i<sizeof(arr)/sizeof(arr[0]); i++)
		{
			if (InlineIsEqualGUID(*arr[i], riid))
				return S_OK;
		}
		return S_FALSE;
	}

// IViewObjectEx
	DECLARE_VIEW_STATUS(VIEWSTATUS_SOLIDBKGND | VIEWSTATUS_OPAQUE)

// IIETabProcControl
public:

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
        if (g_pIOleInPlaceActiveObject)
        {
            g_pIOleInPlaceActiveObject->Release();
        }
	}

    HRESULT OnDraw(ATL_DRAWINFO& di){return S_OK;}
    HWND CreateControlWindow(HWND hwndParent, RECT &rcPos);
    HRESULT __stdcall Invoke(DISPID dispidMember, const IID &riid, LCID lcid, WORD wFlags, DISPPARAMS *pdispparams, VARIANT *pvarResult, EXCEPINFO *pexecpinfo, UINT *puArgErr);
    HRESULT __stdcall SetClientSite(IOleClientSite* pClientSite);
    HRESULT __stdcall Navigate(BSTR bstrUrl);
    HRESULT __stdcall Done();

private:
    CComPtr<IWebBrowser2> m_spInnerWebBrowser;
    CComPtr<IConnectionPoint> m_spCP;
    CComPtr<IWebBrowser2> m_spWebBrowser;
    DWORD m_dwCookie;
};

OBJECT_ENTRY_AUTO(__uuidof(IETabProcControl), CIETabProcControl)
