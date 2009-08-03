// Provider.cpp

#include "StdAfx.h"
#include "Provider.h"
#include "ChooseIssuesDlg.h"

///////////////////////////////////////////////////////////////////////////

CProvider::CProvider()
{
}

HRESULT CProvider::FinalConstruct()
{
	return S_OK;
}

void CProvider::FinalRelease()
{
}

///////////////////////////////////////////////////////////////////////////

HRESULT STDMETHODCALLTYPE CProvider::ValidateParameters(HWND, BSTR, VARIANT_BOOL* valid)
{
	*valid = VARIANT_TRUE;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CProvider::GetLinkText(HWND, BSTR, BSTR* linkText)
{
	*linkText = ::SysAllocString( L"Choose Issue(s)..." );
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CProvider::GetCommitMessage(HWND hParentWnd,
	BSTR parameters, BSTR commonRoot, SAFEARRAY* pathList, BSTR originalMessage, BSTR* newMessage)
{
	BSTR bugID = ::SysAllocString( L"" );
	BSTR* bugIDOut = NULL;
	HRESULT hr = GetCommitMessage2(hParentWnd, parameters,
		NULL, commonRoot, pathList, originalMessage, bugID, bugIDOut, NULL, NULL, newMessage);
	::SysFreeString(bugID);
	return hr;
}

///////////////////////////////////////////////////////////////////////////

HRESULT STDMETHODCALLTYPE CProvider::GetCommitMessage2(HWND hParentWnd,
	BSTR, BSTR, BSTR commonRoot, SAFEARRAY*, BSTR originalMessage, BSTR, BSTR*, SAFEARRAY**, SAFEARRAY**, BSTR* newMessage)
{
	CChooseIssuesDlg dlg(accessor_, CW2A(commonRoot));
	if (dlg.DoModal(hParentWnd) == IDOK)
	{
		LPCTSTR SIGN = "[FlatTracker]:";
		CString message = CW2A(originalMessage);
		int pos = message.Find(SIGN);
		if (pos >= 0)
		{
			message = message.Left(pos);
		}
		std::string text = accessor_.GetIdListText();
		if ( ! text.empty())
		{
			if ((message.GetLength() > 0) && (message.Right(1) != _T("\n")))
			{
				message += _T("\n");
			}
			message += SIGN;
			message += _T(" ");
			message += text.c_str();
		}
		*newMessage = message.AllocSysString();
	}
	else
	{
		*newMessage = originalMessage;
	}
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CProvider::CheckCommit(HWND, BSTR, BSTR, BSTR, SAFEARRAY*, BSTR, BSTR*)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CProvider::OnCommitFinished(HWND hParentWnd, BSTR, SAFEARRAY*, BSTR, ULONG revision, BSTR* error)
{
	if ( ! accessor_.UpdateIssuesTextFile(revision))
	{
		CString err = _T("Update issues text file failed!");
		::MessageBox(hParentWnd, err, NULL, MB_ICONWARNING);
		*error = err.AllocSysString();
	}
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CProvider::HasOptions(VARIANT_BOOL* ret)
{
	*ret = VARIANT_FALSE;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CProvider::ShowOptionsDialog(HWND, BSTR, BSTR*)
{
	return S_OK;
}

///////////////////////////////////////////////////////////////////////////
