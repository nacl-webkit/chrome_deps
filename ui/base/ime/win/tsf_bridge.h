// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_BASE_IME_WIN_TSF_BRIDGE_H_
#define UI_BASE_IME_WIN_TSF_BRIDGE_H_

#include <Windows.h>
#include <msctf.h>

#include "base/basictypes.h"
#include "base/win/scoped_comptr.h"
#include "ui/base/ui_export.h"

namespace ui {
class TextInputClient;

// TSFBridge provides high level IME related operations on top of Text Services
// Framework (TSF). TSFBridge is managed by TLS because TSF related stuff is
// associated with each thread and not allowed to access across thread boundary.
// To be consistent with IMM32 behavior, TSFBridge is shared in the same thread.
// TSFBridge is used by the web content text inputting field, for example
// DisableIME() should be called if a password field is focused.
//
// TSFBridge also manages connectivity between TSFTextStore which is the backend
// of text inputting and current focused TextInputClient.
//
// All methods in this class must be used in UI thread.
class UI_EXPORT TSFBridge {
 public:
  virtual ~TSFBridge();

  // Returns the thread local TSFBridge instance. Initialize() must be called
  // first. Do not cache this pointer and use it after TSFBridge Shutdown().
  static TSFBridge* GetInstance();

  // Sets the thread local instance. Must be called before any calls to
  // GetInstance().
  static bool Initialize();

  // Injects an alternative TSFBridge such as MockTSFBridge for testing. The
  // injected object should be released by the caller. This function returns
  // previous TSFBridge pointer with ownership.
  static TSFBridge* ReplaceForTesting(TSFBridge* bridge);

  // Destroys the thread local instance.
  virtual void Shutdown() = 0;

  // Handles TextInputTypeChanged event. RWHVW is responsible for calling this
  // handler whenever renderer's input text type is changed. Does nothing
  // unless |client| is focused.
  virtual void OnTextInputTypeChanged(TextInputClient* client) = 0;

  // Cancels the ongoing composition if exists.
  // Returns false if an error occures.
  virtual bool CancelComposition() = 0;

  // Sets currently focused TextInputClient.
  // Caller must free |client|.
  virtual void SetFocusedClient(HWND focused_window,
                                TextInputClient* client) = 0;

  // Removes currently focused TextInputClient.
  // Caller must free |client|.
  virtual void RemoveFocusedClient(TextInputClient* client) = 0;

  // Obtains current thread manager.
  virtual base::win::ScopedComPtr<ITfThreadMgr> GetThreadManager() = 0;

 protected:
  // Uses GetInstance() instead.
  TSFBridge();

 private:
  // Releases TLS instance.
  static void Finalize(void* data);

  DISALLOW_COPY_AND_ASSIGN(TSFBridge);
};

}  // namespace ui

#endif  // UI_BASE_IME_WIN_TSF_BRIDGE_H_
