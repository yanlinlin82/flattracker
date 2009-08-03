// ManageIssuesDlg.cpp

#include "StdAfx.h"
#include "ManageIssuesDlg.h"
#include "IssueDlg.h"

///////////////////////////////////////////////////////////////////////////

extern HINSTANCE g_hDll;

///////////////////////////////////////////////////////////////////////////

CManageIssuesDlg::CManageIssuesDlg(IssueAccessor& accessor)
	: accessor_    (accessor)
	, initializing_(false)
	, hListView_   (NULL)
	, modified_    (false)
{
}

LRESULT CManageIssuesDlg::OnInitDialog(UINT, WPARAM, LPARAM, BOOL&)
{
	hListView_ = GetDlgItem(IDC_ISSUE_LIST);
	ListView_SetExtendedListViewStyle(hListView_, LVS_EX_FULLROWSELECT);

	LVCOLUMN lvc = { 0 };
	lvc.mask = LVCF_TEXT | LVCF_WIDTH;
	lvc.pszText = _T("ID");
	lvc.cx = 30;
	ListView_InsertColumn(hListView_, 0, &lvc);
	lvc.pszText = _T("Status");
	lvc.cx = 70;
	ListView_InsertColumn(hListView_, 1, &lvc);
	lvc.pszText = _T("Owner");
	lvc.cx = 80;
	ListView_InsertColumn(hListView_, 2, &lvc);
	lvc.pszText = _T("Title");
	lvc.cx = 350;
	ListView_InsertColumn(hListView_, 3, &lvc);
	lvc.pszText = _T("Labels");
	lvc.cx = 160;
	ListView_InsertColumn(hListView_, 4, &lvc);

	RefreshList();
	return 0;
}

LRESULT CManageIssuesDlg::OnSysCommand(UINT, WPARAM w, LPARAM, BOOL& bHandled)
{
	if (w == SC_CLOSE)
	{
		if (modified_)
		{
			int n = MessageBox(
				_T("Some of the issues have been modified. Do you want to save them now?"),
				_T("Confirm"), MB_ICONINFORMATION | MB_YESNOCANCEL);
			if (n == IDCANCEL)
			{
				bHandled = TRUE;
				return 0;
			}
			else if (n == IDYES)
			{
				SendMessage(WM_COMMAND, IDC_SAVE);
			}
			else
			{
				SendMessage(WM_COMMAND, IDC_RELOAD);
			}
		}
		EndDialog(IDCANCEL);
	}
	return 0;
}

LRESULT CManageIssuesDlg::OnSave(WORD, WORD, HWND, BOOL&)
{
	accessor_.UpdateIssuesTextFile(0);
	modified_ = false;
	return 0;
}

void CManageIssuesDlg::RefreshList()
{
	initializing_ = true;

	const IssueList& issueList = accessor_.GetIssueList();
	int row = 0;
	for (size_t i = 0; i < issueList.GetIssues().size(); i++)
	{
		const IssueItem& item = issueList.GetIssues()[i];

		if (item.GetStatus() == "deleted") continue;

		if ( ! filter_.empty())
		{
			bool match = false;
			for (;;)
			{
				if (item.GetTitle().find(filter_) != std::string::npos)
				{
					match = true;
					break;
				}
				if (item.GetOwner().find(filter_) != std::string::npos)
				{
					match = true;
					break;
				}
				if (item.GetStatus().find(filter_) != std::string::npos)
				{
					match = true;
					break;
				}
				for (std::list<std::string>::const_iterator it = item.GetLabels().begin(); it != item.GetLabels().end(); it++)
				{
					if (it->find(filter_) != std::string::npos)
					{
						match = true;
						break;
					}
				}
				break;
			}
			if ( ! match) continue;
		}

		const int BUFFER_SIZE = 1024;
		TCHAR buffer[BUFFER_SIZE] = _T("");

		wnsprintf(buffer, BUFFER_SIZE, _T("%d"), item.GetId());
		LVITEM lvi = { 0 };
		lvi.mask = LVIF_TEXT | LVIF_PARAM;
		lvi.iItem = row;
		lvi.iSubItem = 0;
		lvi.pszText = buffer;
		lvi.lParam = item.GetId();
		if (row >= ListView_GetItemCount(hListView_))
		{
			ListView_InsertItem(hListView_, &lvi);
		}
		else
		{
			ListView_SetItem(hListView_, &lvi);
		}

		wnsprintf(buffer, BUFFER_SIZE, _T("%s"), (LPCTSTR)CA2T(item.GetStatus().c_str()));
		lvi.mask = LVIF_TEXT;
		lvi.iSubItem = 1;
		lvi.pszText = buffer;
		ListView_SetItem(hListView_, &lvi);

		wnsprintf(buffer, BUFFER_SIZE, _T("%s"), (LPCTSTR)CA2T(item.GetOwner().c_str()));
		lvi.mask = LVIF_TEXT;
		lvi.iSubItem = 2;
		lvi.pszText = buffer;
		ListView_SetItem(hListView_, &lvi);

		wnsprintf(buffer, BUFFER_SIZE, _T("%s"), (LPCTSTR)CA2T(item.GetTitle().c_str()));
		lvi.mask = LVIF_TEXT;
		lvi.iSubItem = 3;
		lvi.pszText = buffer;
		ListView_SetItem(hListView_, &lvi);

		wnsprintf(buffer, BUFFER_SIZE, _T("%s"), (LPCTSTR)CA2T(item.GetLabelsInSingleString().c_str()));
		lvi.mask = LVIF_TEXT;
		lvi.iSubItem = 4;
		lvi.pszText = buffer;
		ListView_SetItem(hListView_, &lvi);

		row++;
	}

	while (row < ListView_GetItemCount(hListView_))
	{
		ListView_DeleteItem(hListView_, row);
	}
	initializing_ = false;
}

LRESULT CManageIssuesDlg::OnOK(WORD, WORD, HWND, BOOL&)
{
	return 0;
}

LRESULT CManageIssuesDlg::OnCancel(WORD, WORD, HWND, BOOL&)
{
	return 0;
}

LRESULT CManageIssuesDlg::OnNMClickIssueList(int, LPNMHDR, BOOL&)
{
	int count = ListView_GetSelectedCount(hListView_);
	::EnableWindow(GetDlgItem(IDC_EDIT), (count == 1));
	::EnableWindow(GetDlgItem(IDC_DELETE), (count > 0));
	return 0;
}

LRESULT CManageIssuesDlg::OnNMDblClkIssueList(int, LPNMHDR, BOOL&)
{
	if (ListView_GetSelectedCount(hListView_) == 1)
	{
		SendMessage(WM_COMMAND, IDC_EDIT);
	}
	return 0;
}

LRESULT CManageIssuesDlg::OnFilterChanged(WORD, WORD, HWND, BOOL&)
{
	CString filter;
	GetDlgItemText(IDC_FILTER, filter);
	filter_ = static_cast<const char*>(CT2A(filter));
	RefreshList();
	return 0;
}

LRESULT CManageIssuesDlg::OnReload(WORD, WORD, HWND, BOOL&)
{
	try
	{
		if ( ! accessor_.LoadIssues())
		{
			throw std::runtime_error("Load issue list failed!");
		}
		modified_ = false;
		RefreshList();
	}
	catch (std::runtime_error& e)
	{
		MessageBox(CA2T(e.what()), NULL, MB_ICONWARNING);
	}
	return 0;
}

LRESULT CManageIssuesDlg::OnAddIssue(WORD, WORD, HWND, BOOL&)
{
	CIssueDlg dlg;
	if (dlg.DoModal() == IDOK)
	{
		int id = accessor_.AddNewIssue(dlg.GetIssue());
		modified_ = true;
		RefreshList();

		int index = -1;
		for (int i = 0; i < ListView_GetItemCount(hListView_); i++)
		{
			LVITEM lvi = { 0 };
			lvi.mask = LVIF_PARAM;
			lvi.iItem = index;
			if (ListView_GetItem(hListView_, &lvi))
			{
				if (lvi.lParam == id)
				{
					index = i;
					break;
				}
			}
		}

		if (index >= 0)
		{
			ListView_EnsureVisible(hListView_, index, FALSE);

			for (int i = 0; i < ListView_GetItemCount(hListView_); i++)
			{
				ListView_SetItemState(hListView_, i, ((i != index) ? 0 : LVIS_SELECTED), LVIS_SELECTED);
			}
		}
	}
	return 0;
}

LRESULT CManageIssuesDlg::OnEditIssue(WORD, WORD, HWND, BOOL&)
{
	int id = 0;
	for (int i = 0; i < ListView_GetItemCount(hListView_); i++)
	{
		if (ListView_GetItemState(hListView_, i, LVIS_SELECTED) == LVIS_SELECTED)
		{
			LVITEM lvi = { 0 };
			lvi.mask = LVIF_PARAM;
			lvi.iItem = i;
			if (ListView_GetItem(hListView_, &lvi))
			{
				id = lvi.lParam;
				break;
			}
		}
	}

	if (id > 0)
	{
		const std::vector<IssueItem>& issues = accessor_.GetIssueList().GetIssues();
		for (size_t i = 0; i < issues.size(); i++)
		{
			if (issues[i].GetId() == id)
			{
				CIssueDlg dlg(issues[i]);
				if (dlg.DoModal() == IDOK)
				{
					accessor_.UpdateIssue(dlg.GetIssue());
					modified_ = true;
					RefreshList();
				}
				break;
			}
		}
	}
	return 0;
}

LRESULT CManageIssuesDlg::OnDeleteIssue(WORD, WORD, HWND, BOOL&)
{
	int n = MessageBox(
		_T("Are you sure to delete the selected issue(s)?"),
		_T("Confirm"), MB_ICONQUESTION | MB_YESNO);
	if (n == IDYES)
	{
		for (int i = 0; i < ListView_GetItemCount(hListView_); i++)
		{
			if (ListView_GetItemState(hListView_, i, LVIS_SELECTED) == LVIS_SELECTED)
			{
				LVITEM lvi = { 0 };
				lvi.mask = LVIF_PARAM;
				lvi.iItem = i;
				if (ListView_GetItem(hListView_, &lvi))
				{
					int id = lvi.lParam;
					accessor_.DeleteIssue(id);
					modified_ = true;
				}
			}
		}
		RefreshList();
	}
	return 0;
}

///////////////////////////////////////////////////////////////////////////
