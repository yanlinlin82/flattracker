// Issues.cpp

#include "StdAfx.h"
#include "Issues.h"
#include <algorithm>
#include <sstream>
#include <iostream>

///////////////////////////////////////////////////////////////////////////

IssueItem::IssueItem(int id, std::string status, std::string owner, std::string labels, std::string title)
	: id_(id)
	, status_(status)
	, owner_(owner)
	, title_(title)
{
	std::string label;
	std::stringstream ss(labels);
	while (std::getline(ss, label, '|'))
	{
		labels_.push_back(label);
	}
}

void IssueItem::SetLabels(const std::vector<std::string>& labels)
{
	labels_.clear();
	labels_.insert(labels_.end(), labels.begin(), labels.end());
}

bool IssueItem::HasLabel(std::string label) const
{
	return (std::find(labels_.begin(), labels_.end(), label) != labels_.end());
}

std::string IssueItem::GetLabelsInSingleString() const
{
	std::stringstream ss;
	for (std::list<std::string>::const_iterator it = labels_.begin(); it != labels_.end(); it++)
	{
		if ( ! ss.str().empty()) ss << ",";
		ss << *it;
	}
	return ss.str();
}

const std::list<std::string>& IssueItem::GetLabels() const
{
	return labels_;
}

///////////////////////////////////////////////////////////////////////////

IssueList::IssueList()
{
}

void IssueList::Clear()
{
	issues_.clear();
}

void IssueList::Append(const IssueItem& item)
{
	issues_.push_back(item);
}

void IssueList::Update(const IssueItem& item)
{
	for (size_t i = 0; i < issues_.size(); i++)
	{
		if (issues_[i].GetId() == item.GetId())
		{
			issues_[i] = item;
			break;
		}
	}
}

void IssueList::Delete(int id)
{
	for (size_t i = 0; i < issues_.size(); i++)
	{
		if (issues_[i].GetId() == id)
		{
			issues_[i].SetStatus("deleted");
			break;
		}
	}
}

int IssueList::GetMaxId() const
{
	int maxId = 0;
	for (size_t i = 0; i < issues_.size(); i++)
	{
		if (maxId < issues_[i].GetId())
		{
			maxId = issues_[i].GetId();
		}
	}
	return maxId;
}

///////////////////////////////////////////////////////////////////////////
