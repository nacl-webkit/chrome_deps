// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/public/common/content_client.h"

#include "base/logging.h"
#include "base/string_piece.h"
//FIXME #include "ui/gfx/image/image.h"
#include <assert.h>
#include "webkit/user_agent/user_agent.h"

//FIXME #if defined(ENABLE_PLUGINS)
#include "webkit/plugins/ppapi/host_globals.h"
//FIXME #endif

namespace content {

static ContentClient* g_client;

void SetContentClient(ContentClient* client) {
  g_client = client;

  // Set the default user agent as provided by the client. We need to make
  // sure this is done before webkit_glue::GetUserAgent() is called (so that
  // the UA doesn't change).
  if (client) {
    assert(0);
//FIXME    webkit_glue::SetUserAgent(client->GetUserAgent(), false);
  }
}

ContentClient* GetContentClient() {
  return g_client;
}

const std::string& GetUserAgent(const GURL& url) {
  assert(0);
  DCHECK(g_client);
//FIXME  return webkit_glue::GetUserAgent(url);
}

webkit::ppapi::HostGlobals* GetHostGlobals() {
//FIXME #if defined(ENABLE_PLUGINS)
  return webkit::ppapi::HostGlobals::Get();
//FIXME #else
//FIXME   return NULL;
//FIXME #endif
}

ContentClient::ContentClient()
    : browser_(NULL), plugin_(NULL), renderer_(NULL), utility_(NULL) {
}

ContentClient::~ContentClient() {
}

bool ContentClient::CanHandleWhileSwappedOut(const IPC::Message& message) {
  return false;
}

std::string ContentClient::GetProduct() const {
  return std::string();
}

std::string ContentClient::GetUserAgent() const {
  return std::string();
}

string16 ContentClient::GetLocalizedString(int message_id) const {
  return string16();
}

//FIXME base::StringPiece ContentClient::GetDataResource(
//FIXME     int resource_id,
//FIXME     ui::ScaleFactor scale_factor) const {
//FIXME   return base::StringPiece();
//FIXME }

base::RefCountedStaticMemory* ContentClient::GetDataResourceBytes(
    int resource_id) const {
  return NULL;
}

//FIXME gfx::Image& ContentClient::GetNativeImageNamed(int resource_id) const {
//FIXME   CR_DEFINE_STATIC_LOCAL(gfx::Image, kEmptyImage, ());
//FIXME   return kEmptyImage;
//FIXME }

#if defined(OS_MACOSX) && !defined(OS_IOS)
bool ContentClient::GetSandboxProfileForSandboxType(
    int sandbox_type,
    int* sandbox_profile_resource_id) const {
  return false;
}

std::string ContentClient::GetCarbonInterposePath() const {
  return std::string();
}
#endif

}  // namespace content
