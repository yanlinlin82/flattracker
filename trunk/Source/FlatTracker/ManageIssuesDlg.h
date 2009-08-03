// ManageIssuesDlg.h

#pragma once

#include "Resource.h"
#include "Issues.h"
#include "IssueAccessor.h"
#include <map>

///////////////////////////////////////////////////////////////////////////

class CManageIssuesDlg
	: public CDialogImpl<CManageIssuesDlg>
{
public:
	explicit CManageIssuesDlg(IssueAccessor& accessor);

public:
	enum { IDD = IDD_MANAGE_ISSUES };

public:
	BEGIN_MSG_MAP(CManageIssuesDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_SYSCOMMAND, OnSysCommand)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
		NOTIFY_HANDLER(IDC_ISSUE_LIST, NM_CLICK, OnNMClickIssueList)
		NOTIFY_HANDLER(IDC_ISSUE_LIST, NM_DBLCLK, OnNMDblClkIssueList)
		COMMAND_HANDLER(IDC_FILTER, EN_CHANGE, OnFilterChanged)
		COMMAND_ID_HANDLER(IDC_RELOAD, OnReload)
		COMMAND_ID_HANDLER(IDC_ADD, OnAddIssue)
		COMMAND_ID_HANDLER(IDC_EDIT, OnEditIssue)
		COMMAND_ID_HANDLER(IDC_DELETE, OnDeleteIssue)
		COMMAND_ID_HANDLER(IDC_SAVE, OnSave)
	END_MSG_MAP()
private:
	LRESULT OnInitDialog(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnSysCommand(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnOK(WORD, WORD, HWND, BOOL&);
	LRESULT OnCancel(WORD, WORD, HWND, BOOL&);
	LRESULT OnNMClickIssueList(int, LPNMHDR, BOOL&);
	LRESULT OnNMDblClkIssueList(int, LPNMHDR, BOOL&);
	LRESULT OnFilterChanged(WORD, WORD, HWND, BOOL&);
	LRESULT OnReload(WORD, WORD, HWND, BOOL&);
	LRESULT OnAddIssue(WORD, WORD, HWND, BOOL&);
	LRESULT OnEditIssue(WORD, WORD, HWND, BOOL&);
	LRESULT OnDeleteIssue(WORD, WORD, HWND, BOOL&);
	LRESULT OnSave(WORD, WORD, HWND, BOOL&);

private:
	void RefreshList();

private:
	IssueAccessor& accessor_;
	bool           initializing_;
	HWND           hListView_;
	std::string    filter_;
	bool           modified_;
};

///////////////////////////////////////////////////////////////////////////
