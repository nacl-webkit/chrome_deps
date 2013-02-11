// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "webkit/plugins/ppapi/url_response_info_util.h"

#include "base/logging.h"
/*FIXME
#include "third_party/WebKit/Source/WebKit/chromium/public/platform/WebHTTPHeaderVisitor.h"
#include "third_party/WebKit/Source/WebKit/chromium/public/platform/WebString.h"
#include "third_party/WebKit/Source/WebKit/chromium/public/platform/WebURL.h"
#include "third_party/WebKit/Source/WebKit/chromium/public/platform/WebURLResponse.h"
#include "webkit/base/file_path_string_conversions.h"
*/
#include "webkit/plugins/ppapi/ppb_file_ref_impl.h"
#include "webkit/glue/webkit_glue.h"
#include <WebCore/ResourceError.h>
#include <WebCore/ResourceRequest.h>
#include <WebCore/ResourceResponse.h>
#include <wtf/text/CString.h>
#include <wtf/text/WTFString.h>

using namespace WebCore;

namespace webkit {
namespace ppapi {

namespace {

class HeaderFlattener {
 public:
  const std::string& buffer() const { return buffer_; }

  virtual void visitHeader(const AtomicString& name, const WTF::String& value) {
    if (!buffer_.empty())
      buffer_.append("\n");
    buffer_.append(name.string().utf8().data());
    buffer_.append(": ");
    buffer_.append(value.utf8().data());
  }

 private:
  std::string buffer_;
};

bool IsRedirect(int32_t status) {
  return status >= 300 && status <= 399;
}

}  // namespace

::ppapi::URLResponseInfoData DataFromWebURLResponse(
    PP_Instance pp_instance,
    const WebCore::ResourceResponse& response) {
  ::ppapi::URLResponseInfoData data;

  data.url = response.url().string().utf8().data();
  data.status_code = response.httpStatusCode();
  data.status_text = response.httpStatusText().utf8().data();
  if (IsRedirect(data.status_code)) {
      data.redirect_url = response.httpHeaderField("Location").utf8().data();
  }

  HeaderFlattener flattener;
  const HTTPHeaderMap& map = response.httpHeaderFields();
  for (HTTPHeaderMap::const_iterator it = map.begin(); it != map.end(); ++it)
      flattener.visitHeader(it->key, it->value);
  data.headers = flattener.buffer();

  std::string file_path = response.downloadFilePath().utf8().data();
  if (!file_path.empty()) {
    scoped_refptr<PPB_FileRef_Impl> file_ref(
        PPB_FileRef_Impl::CreateExternal(
            pp_instance,
            FilePath(file_path),
            std::string()));
    data.body_as_file_ref = file_ref->GetCreateInfo();
    file_ref->GetReference();  // The returned data has one ref for the plugin.
  }
  return data;
}

}  // namespace ppapi
}  // namespace webkit
