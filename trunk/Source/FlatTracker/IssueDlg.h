// IssueDlg.h

#pragma once

#include "Resource.h"
#include "Issues.h"

///////////////////////////////////////////////////////////////////////////

class CIssueDlg
	: public CDialogImpl<CIssueDlg>
{
public:
	CIssueDlg();
	explicit CIssueDlg(const IssueItem& issue);

public:
	enum { IDD = IDD_ISSUE };

public:
	BEGIN_MSG_MAP(CChooseIssuesDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
		COMMAND_ID_HANDLER(IDC_ADD, OnAddLabel)
		COMMAND_ID_HANDLER(IDC_DELETE, OnDeleteLabel)
	END_MSG_MAP()
private:
	LRESULT OnInitDialog(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnOK(WORD, WORD, HWND, BOOL&);
	LRESULT OnCancel(WORD, WORD, HWND, BOOL&);
	LRESULT OnAddLabel(WORD, WORD, HWND, BOOL&);
	LRESULT OnDeleteLabel(WORD, WORD, HWND, BOOL&);

public:
	const IssueItem& GetIssue() const { return issue_; }

private:
	IssueItem issue_;
};

///////////////////////////////////////////////////////////////////////////
