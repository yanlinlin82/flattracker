// ChooseIssuesDlg.cpp

#include "StdAfx.h"
#include "ChooseIssuesDlg.h"
#include "ManageIssuesDlg.h"

///////////////////////////////////////////////////////////////////////////

extern HINSTANCE g_hDll;

///////////////////////////////////////////////////////////////////////////

CChooseIssuesDlg::CChooseIssuesDlg(IssueAccessor& accessor, const char* workPath)
	: accessor_    (accessor)
	, initializing_(false)
	, hListView_   (NULL)
	, workPath_    (workPath)
	, filter_      ("active")
{
}

LRESULT CChooseIssuesDlg::OnInitDialog(UINT, WPARAM, LPARAM, BOOL&)
{
	const IssueList& issueList = accessor_.GetIssueList();
	for (size_t i = 0; i < issueList.GetIssues().size(); i++)
	{
		int id = issueList.GetIssues()[i].GetId();
		checkStatus_[id] = accessor_.GetIssuedChecked(id);
	}

	SetDlgItemText(IDC_FILTER, (LPCTSTR)CA2T(filter_.c_str()));

	hListView_ = GetDlgItem(IDC_ISSUE_LIST);
	ListView_SetExtendedListViewStyle(hListView_, LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES);

	LVCOLUMN lvc = { 0 };
	lvc.mask = LVCF_TEXT | LVCF_WIDTH;
	lvc.pszText = _T("ID");
	lvc.cx = 50;
	ListView_InsertColumn(hListView_, 0, &lvc);
	lvc.pszText = _T("Title");
	lvc.cx = 250;
	ListView_InsertColumn(hListView_, 1, &lvc);
	lvc.pszText = _T("Status");
	lvc.cx = 60;
	ListView_InsertColumn(hListView_, 2, &lvc);
	lvc.pszText = _T("Owner");
	lvc.cx = 70;
	ListView_InsertColumn(hListView_, 3, &lvc);
	lvc.pszText = _T("Labels");
	lvc.cx = 100;
	ListView_InsertColumn(hListView_, 4, &lvc);

	try
	{
		if ( ! accessor_.Initialize(workPath_))
		{
			throw std::runtime_error("Initialize svn client failed!");
		}

		if ( ! accessor_.CheckIssuesInfo())
		{
			int n = MessageBox(
				_T("Can not find issues text file in the repository. Do you want to create now?"),
				_T("Confirm"), MB_ICONQUESTION | MB_OKCANCEL);
			if (n == IDCANCEL)
			{
				PostMessage(WM_COMMAND, IDCANCEL);
				return 0;
			}
			if (n != IDCANCEL)
			{
				if ( ! accessor_.AddIssuesSupport())
				{
					throw std::runtime_error("Add issues support failed. It seems that issues have already existed. Please try again!");
				}
			}
		}
		else
		{
			if ( ! accessor_.LoadIssues())
			{
				throw std::runtime_error("Load issue list failed!");
			}
		}

		RefreshList();
	}
	catch (std::runtime_error& e)
	{
		MessageBox(CA2T(e.what()), NULL, MB_ICONWARNING);
		PostMessage(WM_COMMAND, IDCANCEL);
	}
	return 0;
}

void CChooseIssuesDlg::RefreshList()
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

		wnsprintf(buffer, BUFFER_SIZE, _T("%s"), (LPCTSTR)CA2T(item.GetTitle().c_str()));
		lvi.mask = LVIF_TEXT;
		lvi.iSubItem = 1;
		lvi.pszText = buffer;
		ListView_SetItem(hListView_, &lvi);

		wnsprintf(buffer, BUFFER_SIZE, _T("%s"), (LPCTSTR)CA2T(item.GetStatus().c_str()));
		lvi.mask = LVIF_TEXT;
		lvi.iSubItem = 2;
		lvi.pszText = buffer;
		ListView_SetItem(hListView_, &lvi);

		wnsprintf(buffer, BUFFER_SIZE, _T("%s"), (LPCTSTR)CA2T(item.GetOwner().c_str()));
		lvi.mask = LVIF_TEXT;
		lvi.iSubItem = 3;
		lvi.pszText = buffer;
		ListView_SetItem(hListView_, &lvi);

		wnsprintf(buffer, BUFFER_SIZE, _T("%s"), (LPCTSTR)CA2T(item.GetLabelsInSingleString().c_str()));
		lvi.mask = LVIF_TEXT;
		lvi.iSubItem = 4;
		lvi.pszText = buffer;
		ListView_SetItem(hListView_, &lvi);

		ListView_SetCheckState(hListView_, row, checkStatus_[item.GetId()]);

		row++;
	}

	while (row < ListView_GetItemCount(hListView_))
	{
		ListView_DeleteItem(hListView_, row);
	}
	initializing_ = false;
}

LRESULT CChooseIssuesDlg::OnOK(WORD, WORD, HWND, BOOL&)
{
	for (std::map<int, bool>::const_iterator it = checkStatus_.begin(); it != checkStatus_.end(); it++)
	{
		accessor_.SetIssueChecked(it->first, it->second);
	}
	EndDialog(IDOK);
	return 0;
}

LRESULT CChooseIssuesDlg::OnCancel(WORD, WORD, HWND, BOOL&)
{
	EndDialog(IDCANCEL);
	return 0;
}

LRESULT CChooseIssuesDlg::OnLvnItemchangedIssueList(int, LPNMHDR pNMHDR, BOOL&)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	if ((pNMLV->iItem >= 0) && ((pNMLV->uOldState & LVIS_STATEIMAGEMASK) != (pNMLV->uNewState & LVIS_STATEIMAGEMASK)))
	{
		bool checked = ((((pNMLV->uNewState & LVIS_STATEIMAGEMASK) >> 12) - 1) != 0);

		if (ListView_GetItemState(hListView_, pNMLV->iItem, LVIS_SELECTED) == LVIS_SELECTED)
		{
			for (int i = 0; i < ListView_GetItemCount(hListView_); i++)
			{
				if (ListView_GetItemState(hListView_, i, LVIS_SELECTED) == LVIS_SELECTED)
				{
					ListView_SetCheckState(hListView_, i, checked);
					UpdateIdList(i, checked);
				}
			}
		}
		else
		{
			UpdateIdList(pNMLV->iItem, checked);
		}
	}
	return 0;
}

LRESULT CChooseIssuesDlg::OnManage(WORD, WORD, HWND, BOOL&)
{
	CManageIssuesDlg dlg(accessor_);
	dlg.DoModal();
	RefreshList();
	return 0;
}

LRESULT CChooseIssuesDlg::OnFilterChanged(WORD, WORD, HWND, BOOL&)
{
	CString filter;
	GetDlgItemText(IDC_FILTER, filter);
	filter_ = static_cast<const char*>(CT2A(filter));
	RefreshList();
	return 0;
}

void CChooseIssuesDlg::UpdateIdList(int index, bool checked)
{
	if (initializing_) return;

	LVITEM lvi = { 0 };
	lvi.mask = LVIF_PARAM;
	lvi.iItem = index;
	if (ListView_GetItem(hListView_, &lvi))
	{
		int id = lvi.lParam;
		checkStatus_[id] = checked;
	}
}

///////////////////////////////////////////////////////////////////////////
