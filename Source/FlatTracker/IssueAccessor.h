// IssueAccessor.h

#pragma once

#include "Issues.h"
#include "Subversion.h"

///////////////////////////////////////////////////////////////////////////

class IssueAccessor
{
public:
	IssueAccessor();

public:
	bool Initialize(std::string workPath);

	bool CheckIssuesInfo();
	bool AddIssuesSupport();

	bool LoadIssues();

public:
	const IssueList& GetIssueList() const { return issueList_; }

	void SetIssueChecked(int id, bool checked);
	bool GetIssuedChecked(int id) const;

	int AddNewIssue(const IssueItem& issue);
	void UpdateIssue(const IssueItem& issue);
	void DeleteIssue(int id);

	bool UpdateIssuesTextFile(int revnum);
	std::string GetIdListText() const;

private:
	Subversion client_;

	std::string repositoryRoot_;

	IssueList issueList_;
	std::vector<int> idList_;
};

///////////////////////////////////////////////////////////////////////////
