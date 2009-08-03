// Subversion.h

#pragma once

#include <string>
#include <vector>
#include <iostream>
#include "Issues.h"
#include <svn_pools.h>
#include <svn_client.h>

///////////////////////////////////////////////////////////////////////////

class Subversion
{
public:
	Subversion();
	~Subversion();
private:
	Subversion(const Subversion&); // disable copy
	void operator = (const Subversion&);

public:
	bool Initialize();
	void Uninitialize();

public:
	bool Info(const std::string& url_or_path);
	bool GetRootUrl(const std::string& url_or_path, std::string& rootUrl);

	bool ListFiles(const std::string& path, std::vector<std::string>& files);
	bool ExportFile(const std::string& remoteFile, const std::string& localFile);
	bool ImportFile(const std::string& localFile, const std::string& remoteFile, const std::string& message);
	bool CheckOut(const std::string& url, const std::string& localPath);
	bool Commit(const std::vector<std::string>& files, const std::string& message);

private:
	static svn_error_t* InfoReceiver(void* baton, const char* path, const svn_info_t* info, apr_pool_t* pool);
	static svn_error_t* InfoReceiverForRootUrl(void* baton, const char* path, const svn_info_t* info, apr_pool_t* pool);

private:
	bool              inited_;
	apr_allocator_t*  allocator_;
	apr_pool_t*       pool_;
	svn_client_ctx_t* ctx_;
	svn_auth_baton_t* auth_baton_;

public:
	static const svn_version_t* Version();

private:
	static svn_error_t* userprompt(svn_auth_cred_username_t** cred, void* baton, const char* realm, svn_boolean_t may_save, apr_pool_t* pool);
	static svn_error_t* simpleprompt(svn_auth_cred_simple_t** cred, void* baton, const char* realm, const char* username, svn_boolean_t may_save, apr_pool_t* pool);
	static svn_error_t* sslserverprompt(svn_auth_cred_ssl_server_trust_t** cred_p, void* baton, const char* realm, apr_uint32_t failures, const svn_auth_ssl_server_cert_info_t* cert_info, svn_boolean_t may_save, apr_pool_t* pool);
	static svn_error_t* sslclientprompt(svn_auth_cred_ssl_client_cert_t** cred, void* baton, const char* realm, svn_boolean_t may_save, apr_pool_t* pool);
	static svn_error_t* sslpwprompt(svn_auth_cred_ssl_client_cert_pw_t** cred, void* baton, const char* realm, svn_boolean_t may_save, apr_pool_t* pool);
	static svn_error_t* svn_auth_plaintext_prompt(svn_boolean_t* may_save_plaintext, const char* realmstring, void* baton, apr_pool_t* pool);
	static svn_error_t* svn_auth_plaintext_passphrase_prompt(svn_boolean_t* may_save_plaintext, const char* realmstring, void* baton, apr_pool_t* pool);
};

///////////////////////////////////////////////////////////////////////////

std::ostream& operator << (std::ostream& os, const svn_version_t* version);
std::ostream& operator << (std::ostream& os, svn_node_kind_t kind);

///////////////////////////////////////////////////////////////////////////
