// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <string.h>

#include <iostream>
#include <sstream>

#include "ppapi/c/pp_errors.h"
#include "ppapi/c/ppb_console.h"
#include "ppapi/cpp/instance.h"
#include "ppapi/cpp/module.h"
#include "ppapi/cpp/rect.h"
#include "ppapi/cpp/var.h"

namespace {

class ConsoleDemoInstance : public pp::Instance {
 public:
  ConsoleDemoInstance(PP_Instance instance, pp::Module* module);
  virtual ~ConsoleDemoInstance();

  // pp::Instance implementation (see PPP_Instance).
  virtual void DidChangeView(const pp::Rect& position,
                             const pp::Rect& clip_ignored);

 private:
  // Log an error to the developer console and stderr (though the latter may be
  // closed due to sandboxing or blackholed for other reasons) by creating a
  // temporary of this type and streaming to it.  Example usage:
  // LogError(this).s() << "Hello world: " << 42;
  class LogError {
   public:
    LogError(ConsoleDemoInstance* demo) : demo_(demo) {}
    ~LogError() {
      const std::string& msg = stream_.str();
      demo_->console_if_->Log(demo_->pp_instance(), PP_LOGLEVEL_ERROR,
                              pp::Var(msg).pp_var());
      std::cerr << msg << std::endl;
    }
    // Impl note: it would have been nicer to have LogError derive from
    // std::ostringstream so that it can be streamed to directly, but lookup
    // rules turn streamed string literals to hex pointers on output.
    std::ostringstream& s() { return stream_; }
   private:
    ConsoleDemoInstance* demo_;  // Unowned.
    std::ostringstream stream_;
  };

  // Unowned pointers.
  const PPB_Console* console_if_;
  const PPB_Core* core_if_;
};

ConsoleDemoInstance::ConsoleDemoInstance(PP_Instance instance,
                                                 pp::Module* module)
    : pp::Instance(instance) {
  console_if_ = static_cast<const PPB_Console*>(
      module->GetBrowserInterface(PPB_CONSOLE_INTERFACE));
  core_if_ = static_cast<const PPB_Core*>(
      module->GetBrowserInterface(PPB_CORE_INTERFACE));
  LogError(this).s() << "Created "; 
}

ConsoleDemoInstance::~ConsoleDemoInstance() {
  LogError(this).s() << "Deleted "; 
}

void ConsoleDemoInstance::DidChangeView(
    const pp::Rect& position, const pp::Rect& clip_ignored) {
  if (position.width() == 0 || position.height() == 0)
    return;
  LogError(this).s() << "DidChangeView "; 
}

// This object is the global object representing this plugin library as long
// as it is loaded.
class ConsoleDemoModule : public pp::Module {
 public:
  ConsoleDemoModule() : pp::Module() {}
  virtual ~ConsoleDemoModule() {}

  virtual pp::Instance* CreateInstance(PP_Instance instance) {
    return new ConsoleDemoInstance(instance, this);
  }
};

}  // anonymous namespace

namespace pp {
// Factory function for your specialization of the Module object.
Module* CreateModule() {
  return new ConsoleDemoModule();
}
}  // namespace pp
