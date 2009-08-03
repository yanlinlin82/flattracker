// IssueAccessor.cpp

#include "StdAfx.h"
#include "IssueAccessor.h"
#include <algorithm>
#include <fstream>
#include <sstream>

///////////////////////////////////////////////////////////////////////////

std::vector<std::string> split(std::string text, char c)
{
	std::vector<std::string> result;
	std::stringstream ss(text);
	std::string line;
	while (std::getline(ss, line, c))
	{
		result.push_back(line);
	}
	return result;
}

///////////////////////////////////////////////////////////////////////////

IssueAccessor::IssueAccessor()
{
}

bool IssueAccessor::Initialize(std::string workPath)
{
	std::cout << "SVN client version: " << Subversion::Version() << std::endl;

	if ( ! client_.Initialize())
	{
		return false;
	}
	if ( ! client_.GetRootUrl(workPath, repositoryRoot_))
	{
		return false;
	}
	return true;
}

bool IssueAccessor::CheckIssuesInfo()
{
	std::vector<std::string> files;
	if ( ! client_.ListFiles(repositoryRoot_, files))
	{
		return false;
	}
	if (std::find(files.begin(), files.end(), "issues") == files.end())
	{
		return false;
	}

	if ( ! client_.ListFiles(repositoryRoot_ + "/issues", files))
	{
		return false;
	}
	if (std::find(files.begin(), files.end(), "issues.txt") == files.end())
	{
		return false;
	}
	return true;
}

bool IssueAccessor::AddIssuesSupport()
{
	TCHAR buffer[MAX_PATH] = _T("");
	::GetTempPath(MAX_PATH, buffer);
	std::string tempPath = (const char*)CT2A(buffer);

	std::string filename = tempPath + "/issues.txt";

	std::ofstream file(filename.c_str(), std::ios::out);
	if ( ! file.is_open())
	{
		return false;
	}
	file << "#id,status,owner,label,title" << std::endl;
	file.close();

	if ( ! client_.ImportFile(filename, repositoryRoot_ + "/issues/issues.txt", "Initial issues support."))
	{
		return false;
	}

	::DeleteFile(CA2T(filename.c_str()));
	return true;
}

bool IssueAccessor::LoadIssues()
{
	TCHAR buffer[MAX_PATH] = _T("");
	::GetTempPath(MAX_PATH, buffer);
	std::string tempPath = (const char*)CT2A(buffer);

	std::string filename = tempPath + "/issues.txt";

	if ( ! client_.ExportFile(repositoryRoot_ + "/issues/issues.txt", filename))
	{
		return false;
	}

	std::ifstream file(filename.c_str(), std::ios::in);
	if ( ! file.is_open())
	{
		return false;
	}

	issueList_.Clear();
	std::string line;
	while (std::getline(file, line, '\n'))
	{
		if (line.empty()) continue;

		if (line.substr(line.size() - 1) == "\r")
		{
			line = line.substr(0, line.size() - 1);
		}
		if (line.empty()) continue;

		if (line.substr(0, 1) == "#") continue;

		std::vector<std::string> result = split(line, ',');
		if (result.size() < 5) continue;
		int id = atoi(result[0].c_str());
		std::string status = result[1];
		std::string owner  = result[2];
		std::string labels = result[3];
		std::string title  = result[4];
		issueList_.Append( IssueItem(id, status, owner, labels, title) );
	}
	file.close();

	::DeleteFile(CA2T(filename.c_str()));
	return true;
}

void IssueAccessor::SetIssueChecked(int id, bool checked)
{
	if (checked)
	{
		if ( ! GetIssuedChecked(id))
		{
			idList_.push_back(id);
		}
	}
	else
	{
		std::vector<int>::iterator it = std::find(idList_.begin(), idList_.end(), id);
		if (it != idList_.end())
		{
			idList_.erase(it);
		}
	}
}

bool IssueAccessor::GetIssuedChecked(int id) const
{
	return (std::find(idList_.begin(), idList_.end(), id) != idList_.end());
}

int IssueAccessor::AddNewIssue(const IssueItem& issue)
{
	int newId = issueList_.GetMaxId() + 1;
	issueList_.Append( IssueItem(newId, issue.GetStatus(), issue.GetOwner(), issue.GetLabelsInSingleString(), issue.GetTitle()) );
	return newId;
}

void IssueAccessor::UpdateIssue(const IssueItem& issue)
{
	issueList_.Update(issue);
}

void IssueAccessor::DeleteIssue(int id)
{
	issueList_.Delete(id);
}

bool IssueAccessor::UpdateIssuesTextFile(int revnum)
{
	if ((revnum > 0) && idList_.empty()) return true;

	TCHAR buffer[MAX_PATH] = _T("");
	::GetTempPath(MAX_PATH, buffer);
	std::string tempPath = (const char*)CT2A(buffer);

	if ( ! client_.CheckOut(repositoryRoot_ + "/issues/", tempPath + "\\issues"))
	{
		return false;
	}

	std::ofstream file((tempPath + "\\issues\\issues.txt").c_str(), std::ios::out);
	if ( ! file.is_open())
	{
		return false;
	}

	file << "#id,status,owner,label,title" << std::endl;

	const std::vector<IssueItem>& issues = issueList_.GetIssues();
	for (size_t i = 0; i < issues.size(); i++)
	{
		file << issues[i].GetId();
		file << ",";
		if ((revnum > 0) && (std::find(idList_.begin(), idList_.end(), issues[i].GetId()) != idList_.end()))
		{
			file << "resolved";
		}
		else
		{
			file << issues[i].GetStatus();
		}
		file << ",";
		file << issues[i].GetOwner();
		file << ",";
		for (std::list<std::string>::const_iterator it = issues[i].GetLabels().begin(); it != issues[i].GetLabels().end(); it++)
		{
			if (it != issues[i].GetLabels().begin()) file << "|";
			file << *it;
		}
		file << ",";
		file << issues[i].GetTitle();
		file << std::endl;
	}

	file.close();

	std::vector<std::string> files;
	files.push_back(tempPath + "\\issues\\issues.txt");
	std::stringstream ss;
	ss << "Update issues text file";
	if (revnum > 0)
	{
		ss << " for revision " << revnum;
	}
	ss << ".";
	if ( ! client_.Commit(files, ss.str()))
	{
		return false;
	}

	TCHAR path[MAX_PATH + 1] = { 0 };
	lstrcpyn(path, CA2T((tempPath + "\\issues").c_str()), MAX_PATH);
	SHFILEOPSTRUCT fileOp = { 0 };
	fileOp.fFlags = FOF_NOCONFIRMATION;
	fileOp.wFunc = FO_DELETE;
	fileOp.pFrom = path;
	::SHFileOperation(&fileOp);
	return true;
}

std::string IssueAccessor::GetIdListText() const
{
	std::stringstream ss;
	for (size_t i = 0; i < idList_.size(); i++)
	{
		if (i > 0) ss << ", ";
		ss << idList_[i];
	}
	return ss.str();
}

///////////////////////////////////////////////////////////////////////////
