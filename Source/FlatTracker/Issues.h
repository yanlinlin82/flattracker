// Issues.h

#pragma once

#include <vector>
#include <string>
#include <list>

///////////////////////////////////////////////////////////////////////////

class IssueItem
{
public:
	IssueItem(int id, std::string status, std::string owner, std::string labels, std::string title);

public:
	int GetId() const { return id_; }
	std::string GetStatus() const { return status_; }
	std::string GetOwner() const { return owner_; }
	std::string GetTitle() const { return title_; }

public:
	void SetTitle(const std::string& title) { title_ = title; }
	void SetStatus(const std::string& status) { status_ = status; }
	void SetOwner(const std::string& owner) { owner_ = owner; }

public:
	void SetLabels(const std::vector<std::string>& labels);
	bool HasLabel(std::string label) const;

	std::string GetLabelsInSingleString() const;

	const std::list<std::string>& GetLabels() const;

private:
	int id_;
	std::string status_;
	std::string owner_;
	std::list<std::string> labels_;
	std::string title_;
};


///////////////////////////////////////////////////////////////////////////

class IssueList
{
public:
	IssueList();

public:
	void Clear();
	void Append(const IssueItem& item);
	void Update(const IssueItem& item);
	void Delete(int id);

	const std::vector<IssueItem>& GetIssues() const { return issues_; }

	int GetMaxId() const;

private:
	std::vector<IssueItem> issues_;
};

///////////////////////////////////////////////////////////////////////////
