// ChooseIssuesDlg.h

#pragma once

#include "Resource.h"
#include "Issues.h"
#include "IssueAccessor.h"
#include <map>

///////////////////////////////////////////////////////////////////////////

class CChooseIssuesDlg
	: public CDialogImpl<CChooseIssuesDlg>
{
public:
	CChooseIssuesDlg(IssueAccessor& accessor, const char* workPath);

public:
	enum { IDD = IDD_CHOOSE_ISSUES };

public:
	BEGIN_MSG_MAP(CChooseIssuesDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
		NOTIFY_HANDLER(IDC_ISSUE_LIST, LVN_ITEMCHANGED, OnLvnItemchangedIssueList)
		COMMAND_ID_HANDLER(IDC_MANAGE, OnManage)
		COMMAND_HANDLER(IDC_FILTER, EN_CHANGE, OnFilterChanged)
	END_MSG_MAP()
private:
	LRESULT OnInitDialog(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnOK(WORD, WORD, HWND, BOOL&);
	LRESULT OnCancel(WORD, WORD, HWND, BOOL&);
	LRESULT OnLvnItemchangedIssueList(int, LPNMHDR, BOOL&);
	LRESULT OnManage(WORD, WORD, HWND, BOOL&);
	LRESULT OnFilterChanged(WORD, WORD, HWND, BOOL&);

private:
	void UpdateIdList(int index, bool checked);
	void RefreshList();

private:
	IssueAccessor& accessor_;
	bool           initializing_;
	HWND           hListView_;
	std::string    workPath_;
	std::string    filter_;
	std::map<int, bool> checkStatus_;
};

///////////////////////////////////////////////////////////////////////////
