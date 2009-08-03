// IssueDlg.cpp

#include "StdAfx.h"
#include "IssueDlg.h"

///////////////////////////////////////////////////////////////////////////

CIssueDlg::CIssueDlg()
	: issue_(0, "active", "", "", "")
{
}

CIssueDlg::CIssueDlg(const IssueItem& issue)
	: issue_(issue)
{
}

///////////////////////////////////////////////////////////////////////////

LRESULT CIssueDlg::OnInitDialog(UINT, WPARAM, LPARAM, BOOL&)
{
	SetWindowText((issue_.GetId() == 0) ? _T("Add Issue") : _T("Edit Issue"));

	SetDlgItemText(IDC_TITLE, (LPCTSTR)CA2T(issue_.GetTitle().c_str()));
	SetDlgItemText(IDC_OWNER, (LPCTSTR)CA2T(issue_.GetOwner().c_str()));

	if (issue_.GetStatus() == "active")
	{
		::SendMessage(GetDlgItem(IDC_STATUS_ACTIVE), BM_SETCHECK, BST_CHECKED, 0);
	}
	else if (issue_.GetStatus() == "resolved")
	{
		::SendMessage(GetDlgItem(IDC_STATUS_RESOLVED), BM_SETCHECK, BST_CHECKED, 0);
	}
	else if (issue_.GetStatus() == "closed")
	{
		::SendMessage(GetDlgItem(IDC_STATUS_CLOSED), BM_SETCHECK, BST_CHECKED, 0);
	}

	for (std::list<std::string>::const_iterator it = issue_.GetLabels().begin(); it != issue_.GetLabels().end(); it++)
	{
		::SendMessage(GetDlgItem(IDC_LABELS), CB_INSERTSTRING, (WPARAM)-1, (LPARAM)(LPCTSTR)CA2T(it->c_str()));
	}
	return 0;
}

LRESULT CIssueDlg::OnOK(WORD, WORD, HWND, BOOL&)
{
	CString text;
	GetDlgItemText(IDC_TITLE, text);
	issue_.SetTitle((const char*)CT2A(text));

	GetDlgItemText(IDC_OWNER, text);
	issue_.SetOwner((const char*)CT2A(text));

	if (::SendMessage(GetDlgItem(IDC_STATUS_ACTIVE), BM_GETCHECK, 0, 0) == BST_CHECKED)
	{
		issue_.SetStatus("active");
	}
	else if (::SendMessage(GetDlgItem(IDC_STATUS_RESOLVED), BM_GETCHECK, 0, 0) == BST_CHECKED)
	{
		issue_.SetStatus("resolved");
	}
	else if (::SendMessage(GetDlgItem(IDC_STATUS_CLOSED), BM_GETCHECK, 0, 0) == BST_CHECKED)
	{
		issue_.SetStatus("closed");
	}

	std::vector<std::string> labels;
	for (int i = 0; i < ::SendMessage(GetDlgItem(IDC_LABELS), CB_GETCOUNT, 0, 0); i++)
	{
		LPTSTR buffer = text.GetBuffer(::SendMessage(GetDlgItem(IDC_LABELS), CB_GETLBTEXTLEN, 0, 0));
		::SendMessage(GetDlgItem(IDC_LABELS), CB_GETLBTEXT, i, (LPARAM)buffer);
		text.ReleaseBuffer();
		labels.push_back((const char*)CT2A(text));
	}
	issue_.SetLabels(labels);

	EndDialog(IDOK);
	return 0;
}

LRESULT CIssueDlg::OnCancel(WORD, WORD, HWND, BOOL&)
{
	EndDialog(IDCANCEL);
	return 0;
}

LRESULT CIssueDlg::OnAddLabel(WORD, WORD, HWND, BOOL&)
{
	CString text;
	GetDlgItemText(IDC_LABELS, text);
	::SendMessage(GetDlgItem(IDC_LABELS), CB_INSERTSTRING, (WPARAM)-1, (LPARAM)(LPCTSTR)text);
	::SetFocus(GetDlgItem(IDC_LABELS));
	return 0;
}

LRESULT CIssueDlg::OnDeleteLabel(WORD, WORD, HWND, BOOL&)
{
	int index = ::SendMessage(GetDlgItem(IDC_LABELS), CB_GETCURSEL, 0, 0);
	if (index >= 0)
	{
		::SendMessage(GetDlgItem(IDC_LABELS), CB_DELETESTRING, index, 0);
	}
	::SetFocus(GetDlgItem(IDC_LABELS));
	return 0;
}

///////////////////////////////////////////////////////////////////////////
