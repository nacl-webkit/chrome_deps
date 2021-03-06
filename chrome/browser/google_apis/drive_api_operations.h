// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_GOOGLE_APIS_DRIVE_API_OPERATIONS_H_
#define CHROME_BROWSER_GOOGLE_APIS_DRIVE_API_OPERATIONS_H_

#include <string>

#include "chrome/browser/google_apis/base_operations.h"
#include "chrome/browser/google_apis/drive_api_url_generator.h"

namespace net {
class URLRequestContextGetter;
}  // namespace net

namespace google_apis {

//============================== GetAboutOperation =============================

// This class performs the operation for fetching About data.
class GetAboutOperation : public GetDataOperation {
 public:
  GetAboutOperation(OperationRegistry* registry,
                    net::URLRequestContextGetter* url_request_context_getter,
                    const DriveApiUrlGenerator& url_generator,
                    const GetDataCallback& callback);
  virtual ~GetAboutOperation();

 protected:
  // Overridden from GetDataOperation.
  virtual GURL GetURL() const OVERRIDE;

 private:
  const DriveApiUrlGenerator url_generator_;

  DISALLOW_COPY_AND_ASSIGN(GetAboutOperation);
};

//============================= GetApplistOperation ============================

// This class performs the operation for fetching Applist.
class GetApplistOperation : public GetDataOperation {
 public:
  GetApplistOperation(OperationRegistry* registry,
                      net::URLRequestContextGetter* url_request_context_getter,
                      const DriveApiUrlGenerator& url_generator,
                      const GetDataCallback& callback);
  virtual ~GetApplistOperation();

 protected:
  // Overridden from GetDataOperation.
  virtual GURL GetURL() const OVERRIDE;

 private:
  const DriveApiUrlGenerator url_generator_;

  DISALLOW_COPY_AND_ASSIGN(GetApplistOperation);
};

//============================ GetChangelistOperation ==========================

// This class performs the operation for fetching changelist.
class GetChangelistOperation : public GetDataOperation {
 public:
  // |start_changestamp| specifies the starting point of change list or 0 if
  // all changes are necessary.
  // |url| specifies URL for document feed fetching operation. If empty URL is
  // passed, the default URL is used and returns the first page of the result.
  // When non-first page result is requested, |url| should be specified.
  GetChangelistOperation(
      OperationRegistry* registry,
      net::URLRequestContextGetter* url_request_context_getter,
      const DriveApiUrlGenerator& url_generator,
      const GURL& url,
      int64 start_changestamp,
      const GetDataCallback& callback);
  virtual ~GetChangelistOperation();

 protected:
  // Overridden from GetDataOperation.
  virtual GURL GetURL() const OVERRIDE;

 private:
  const DriveApiUrlGenerator url_generator_;
  GURL url_;
  int64 start_changestamp_;

  DISALLOW_COPY_AND_ASSIGN(GetChangelistOperation);
};

//============================= GetFlielistOperation ===========================

// This class performs the operation for fetching Filelist.
class GetFilelistOperation : public GetDataOperation {
 public:
  GetFilelistOperation(
      OperationRegistry* registry,
      net::URLRequestContextGetter* url_request_context_getter,
      const DriveApiUrlGenerator& url_generator,
      const GURL& url,
      const std::string& search_string,
      const GetDataCallback& callback);
  virtual ~GetFilelistOperation();

 protected:
  // Overridden from GetDataOperation.
  virtual GURL GetURL() const OVERRIDE;

 private:
  const DriveApiUrlGenerator url_generator_;
  GURL url_;
  std::string search_string_;

  DISALLOW_COPY_AND_ASSIGN(GetFilelistOperation);
};

//=============================== GetFlieOperation =============================

// This class performs the operation for fetching a file.
class GetFileOperation : public GetDataOperation {
 public:
  GetFileOperation(OperationRegistry* registry,
                   net::URLRequestContextGetter* url_request_context_getter,
                   const DriveApiUrlGenerator& url_generator,
                   const std::string& file_id,
                   const GetDataCallback& callback);
  virtual ~GetFileOperation();

 protected:
  // Overridden from GetDataOperation.
  virtual GURL GetURL() const OVERRIDE;

 private:
  const DriveApiUrlGenerator url_generator_;
  std::string file_id_;

  DISALLOW_COPY_AND_ASSIGN(GetFileOperation);
};

// This namespace is introduced to avoid class name confliction between
// the operations for Drive API v2 and GData WAPI for transition.
// And, when the migration is done and GData WAPI's code is cleaned up,
// classes inside this namespace should be moved to the google_apis namespace.
// TODO(hidehiko): Get rid of this namespace after the migration.
namespace drive {

//========================== CreateDirectoryOperation ==========================

// This class performs the operation for creating a directory.
class CreateDirectoryOperation : public GetDataOperation {
 public:
  CreateDirectoryOperation(
      OperationRegistry* registry,
      net::URLRequestContextGetter* url_request_context_getter,
      const DriveApiUrlGenerator& url_generator,
      const std::string& parent_resource_id,
      const std::string& directory_name,
      const GetDataCallback& callback);
  virtual ~CreateDirectoryOperation();

 protected:
  // Overridden from GetDataOperation.
  virtual GURL GetURL() const OVERRIDE;
  virtual net::URLFetcher::RequestType GetRequestType() const OVERRIDE;
  virtual bool GetContentData(std::string* upload_content_type,
                              std::string* upload_content) OVERRIDE;

 private:
  const DriveApiUrlGenerator url_generator_;
  const std::string parent_resource_id_;
  const std::string directory_name_;

  DISALLOW_COPY_AND_ASSIGN(CreateDirectoryOperation);
};

}  // namespace drive
}  // namespace google_apis

#endif  // CHROME_BROWSER_GOOGLE_APIS_DRIVE_API_OPERATIONS_H_
