// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_PUBLIC_BROWSER_JAVASCRIPT_DIALOG_MANAGER_H_
#define CONTENT_PUBLIC_BROWSER_JAVASCRIPT_DIALOG_MANAGER_H_

#include <string>

#include "base/callback.h"
#include "base/string16.h"
#include "content/common/content_export.h"
#include "content/public/common/javascript_message_type.h"
#include "googleurl/src/gurl.h"
#include "ui/gfx/native_widget_types.h"

namespace content {

class WebContents;

// An interface consisting of methods that can be called to produce and manage
// JavaScript dialogs.
class JavaScriptDialogManager {
 public:
  typedef base::Callback<void(bool /* success */,
                              const string16& /* user_input */)>
                                  DialogClosedCallback;

  // Displays a JavaScript dialog. |did_suppress_message| will not be nil; if
  // |true| is returned in it, the caller will handle faking the reply.
  virtual void RunJavaScriptDialog(
      WebContents* web_contents,
      const GURL& origin_url,
      const std::string& accept_lang,
      JavaScriptMessageType javascript_message_type,
      const string16& message_text,
      const string16& default_prompt_text,
      const DialogClosedCallback& callback,
      bool* did_suppress_message) = 0;

  // Displays a dialog asking the user if they want to leave a page.
  virtual void RunBeforeUnloadDialog(WebContents* web_contents,
                                     const string16& message_text,
                                     bool is_reload,
                                     const DialogClosedCallback& callback) = 0;

  // Cancels all pending dialogs and resets any saved JavaScript dialog state
  // for the given WebContents.
  virtual void ResetJavaScriptState(WebContents* web_contents) = 0;

  virtual ~JavaScriptDialogManager() {}
};

}  // namespace content

#endif  // CONTENT_PUBLIC_BROWSER_JAVASCRIPT_DIALOG_MANAGER_H_
