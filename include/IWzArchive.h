#pragma once
#include <OAIdl.h>

class IWzArchive
{
public:
	virtual HRESULT QueryInterface(IUnknown*, const _GUID*, void**) = 0;
	virtual unsigned int AddRef(IUnknown*) = 0;
	virtual unsigned int Release(IUnknown*) = 0;
	virtual HRESULT get_loading(IWzArchive*, int*) = 0;
	virtual HRESULT raw_Read(IWzArchive*, unsigned __int8*, unsigned int, unsigned int*) = 0;
	virtual HRESULT raw_Write(IWzArchive*, unsigned __int8*, unsigned int, unsigned int*) = 0;
	virtual HRESULT get_absoluteUOL(IWzArchive*, wchar_t**) = 0;
	virtual HRESULT put_absoluteUOL(IWzArchive*, wchar_t*) = 0;
	virtual HRESULT get_position(IWzArchive*, unsigned int*) = 0;
	virtual HRESULT get_context(IWzArchive*, tagVARIANT*) = 0;
	virtual HRESULT put_context(IWzArchive*, tagVARIANT) = 0;
};

