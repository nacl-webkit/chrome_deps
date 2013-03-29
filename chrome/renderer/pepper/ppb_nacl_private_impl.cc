// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/renderer/pepper/ppb_nacl_private_impl.h"
///*
//FIXME:

#include "config.h"
#ifndef DISABLE_NACL

#include "base/command_line.h"
#include "base/lazy_instance.h"
#include "base/logging.h"
#include "base/rand_util.h"
#include "chrome/common/chrome_switches.h"
//#include "chrome/common/render_messages.h"
#include "chrome/common/nacl_types.h"
//#include "chrome/renderer/chrome_render_process_observer.h"
//#include "content/public/common/content_client.h"
#include "content/public/common/content_switches.h"
#include "content/public/common/sandbox_init.h"
#include "content/public/renderer/renderer_ppapi_host.h"
//#include "content/public/renderer/render_thread.h"
//#include "content/public/renderer/render_view.h"
//#include "ipc/ipc_sync_message_filter.h"
#include "ppapi/c/pp_bool.h"
#include "ppapi/c/private/pp_file_handle.h"
#include "ppapi/native_client/src/trusted/plugin/nacl_entry_points.h"
#include "ppapi/shared_impl/ppapi_preferences.h"
//#include "third_party/WebKit/Source/WebKit/chromium/public/WebDocument.h"
//#include "third_party/WebKit/Source/WebKit/chromium/public/WebElement.h"
//#include "third_party/WebKit/Source/WebKit/chromium/public/WebFrame.h"
//#include "third_party/WebKit/Source/WebKit/chromium/public/WebPluginContainer.h"
//#include "third_party/WebKit/Source/WebKit/chromium/public/WebView.h"
#include <WebCore/Document.h>
#include <WebCore/Element.h>
#include <WebCore/Frame.h>
#include "webkit/plugins/ppapi/host_globals.h"
#include "webkit/plugins/ppapi/plugin_module.h"
#include "webkit/plugins/ppapi/ppapi_plugin_instance.h"

#include "WebProcess.h"

using namespace WebCore;

namespace {

// This allows us to send requests from background threads.
// E.g., to do LaunchSelLdr for helper nexes (which is done synchronously),
// in a background thread, to avoid jank.
//FIXME base::LazyInstance<scoped_refptr<IPC::SyncMessageFilter> >
//FIXME     g_background_thread_sender = LAZY_INSTANCE_INITIALIZER;

struct InstanceInfo {
  InstanceInfo() : plugin_pid(base::kNullProcessId), plugin_child_id(0) {}
  GURL url;
  ppapi::PpapiPermissions permissions;
  base::ProcessId plugin_pid;
  int plugin_child_id;
  //FIXME IPC::ChannelHandle channel_handle;
};

typedef std::map<PP_Instance, InstanceInfo> InstanceInfoMap;

base::LazyInstance<InstanceInfoMap> g_instance_info =
    LAZY_INSTANCE_INITIALIZER;

static int GetRoutingID(PP_Instance instance) {
  ASSERT_NOT_REACHED();
  return 0;
//FIXME  // Check that we are on the main renderer thread.
//FIXME  DCHECK(content::RenderThread::Get());
//FIXME  content::RendererPpapiHost *host =
//FIXME      content::RendererPpapiHost::GetForPPInstance(instance);
//FIXME  if (!host)
//FIXME    return 0;
//FIXME  return host->GetRoutingIDForWidget(instance);
}

// Launch NaCl's sel_ldr process.
PP_NaClResult LaunchSelLdr(PP_Instance instance,
                           const char* alleged_url,
                           PP_Bool uses_irt,
                           PP_Bool uses_ppapi,
                           PP_Bool enable_ppapi_dev,
                           void* imc_handle) {
  nacl::FileDescriptor result_socket;
//FIXME  IPC::Sender* sender = content::RenderThread::Get();
//FIXME  if (sender == NULL)
//FIXME    sender = g_background_thread_sender.Pointer()->get();
//FIXME
//FIXME  int routing_id = 0;
//FIXME  // If the nexe uses ppapi APIs, we need a routing ID.
//FIXME  // To get the routing ID, we must be on the main thread.
//FIXME  // Some nexes do not use ppapi and launch from the background thread,
//FIXME  // so those nexes can skip finding a routing_id.
//FIXME  if (uses_ppapi) {
//FIXME    routing_id = GetRoutingID(instance);
//FIXME    if (!routing_id)
//FIXME      return PP_NACL_FAILED;
//FIXME  }
//FIXME
//FIXME  InstanceInfo instance_info;
//FIXME  instance_info.url = GURL(alleged_url);
//FIXME
//FIXME  uint32_t perm_bits = ppapi::PERMISSION_NONE;
//FIXME  // Conditionally block 'Dev' interfaces. We do this for the NaCl process, so
//FIXME  // it's clearer to developers when they are using 'Dev' inappropriately. We
//FIXME  // must also check on the trusted side of the proxy.
//FIXME  if (enable_ppapi_dev)
//FIXME    perm_bits |= ppapi::PERMISSION_DEV;
//FIXME  instance_info.permissions =
//FIXME      ppapi::PpapiPermissions::GetForCommandLine(perm_bits);
//FIXME
//FIXME  if (!sender->Send(new ChromeViewHostMsg_LaunchNaCl(
//FIXME          nacl::NaClLaunchParams(instance_info.url.spec(),
//FIXME                                 routing_id,
//FIXME                                 perm_bits,
//FIXME                                 PP_ToBool(uses_irt)),
//FIXME          &result_socket,
//FIXME          &instance_info.channel_handle,
//FIXME          &instance_info.plugin_pid,
//FIXME          &instance_info.plugin_child_id))) {
//FIXME    return PP_NACL_FAILED;
//FIXME  }
  if (!WebKit::WebProcess::shared().launchNaCl(result_socket.fd)) {
    DLOG(ERROR) << "WebKit::WebProcess::shared().launchNaCl() failed";
    return PP_NACL_FAILED;
  }

//FIXME  // Don't save instance_info if channel handle is invalid.
//FIXME  bool invalid_handle = instance_info.channel_handle.name.empty();
//FIXME#if defined(OS_POSIX)
//FIXME  if (!invalid_handle)
//FIXME    invalid_handle = (instance_info.channel_handle.socket.fd == -1);
//FIXME#endif
//FIXME  if (!invalid_handle)
//FIXME    g_instance_info.Get()[instance] = instance_info;
//FIXME
  *(static_cast<nacl::Handle*>(imc_handle)) =
      nacl::ToNativeHandle(result_socket);

  return PP_NACL_OK;
}

PP_NaClResult StartPpapiProxy(PP_Instance instance) {
  InstanceInfoMap& map = g_instance_info.Get();
  InstanceInfoMap::iterator it = map.find(instance);
  if (it == map.end()) {
    DLOG(ERROR) << "Could not find instance ID";
    return PP_NACL_FAILED;
  }
  InstanceInfo instance_info = it->second;
  map.erase(it);

  ASSERT_NOT_REACHED();
//FIXME  webkit::ppapi::PluginInstance* plugin_instance =
//FIXME      content::GetHostGlobals()->GetInstance(instance);
//FIXME  if (!plugin_instance) {
//FIXME    DLOG(ERROR) << "GetInstance() failed";
//FIXME    return PP_NACL_ERROR_MODULE;
//FIXME  }
//FIXME
//FIXME  // Create a new module for each instance of the NaCl plugin that is using
//FIXME  // the IPC based out-of-process proxy. We can't use the existing module,
//FIXME  // because it is configured for the in-process NaCl plugin, and we must
//FIXME  // keep it that way to allow the page to create other instances.
//FIXME  webkit::ppapi::PluginModule* plugin_module = plugin_instance->module();
//FIXME  scoped_refptr<webkit::ppapi::PluginModule> nacl_plugin_module(
//FIXME      plugin_module->CreateModuleForNaClInstance());
//FIXME
//FIXME  content::RendererPpapiHost* renderer_ppapi_host =
//FIXME      content::RendererPpapiHost::CreateExternalPluginModule(
//FIXME          nacl_plugin_module,
//FIXME          plugin_instance,
//FIXME          FilePath().AppendASCII(instance_info.url.spec()),
//FIXME          instance_info.permissions,
//FIXME          instance_info.channel_handle,
//FIXME          instance_info.plugin_pid,
//FIXME          instance_info.plugin_child_id);
//FIXME  if (!renderer_ppapi_host) {
//FIXME    DLOG(ERROR) << "CreateExternalPluginModule() failed";
    return PP_NACL_ERROR_MODULE;
//FIXME  }
//FIXME
//FIXME  // Finally, switch the instance to the proxy.
//FIXME  return nacl_plugin_module->InitAsProxiedNaCl(plugin_instance);
}

int UrandomFD(void) {
#if defined(OS_POSIX)
  return base::GetUrandomFD();
#else
  return -1;
#endif
}

PP_Bool Are3DInterfacesDisabled() {
  return PP_FromBool(CommandLine::ForCurrentProcess()->HasSwitch(
                         switches::kDisable3DAPIs));
}

void EnableBackgroundSelLdrLaunch() {
  ASSERT_NOT_REACHED();
//FIXME  g_background_thread_sender.Get() =
//FIXME      content::RenderThread::Get()->GetSyncMessageFilter();
}

int32_t BrokerDuplicateHandle(PP_FileHandle source_handle,
                              uint32_t process_id,
                              PP_FileHandle* target_handle,
                              uint32_t desired_access,
                              uint32_t options) {
#if defined(OS_WIN)
  return content::BrokerDuplicateHandle(source_handle, process_id,
                                        target_handle, desired_access,
                                        options);
#else
  return 0;
#endif
}

PP_FileHandle GetReadonlyPnaclFD(const char* filename) {
    ASSERT_NOT_REACHED();
//FIXME  IPC::PlatformFileForTransit out_fd = IPC::InvalidPlatformFileForTransit();
//FIXME  IPC::Sender* sender = content::RenderThread::Get();
//FIXME  if (sender == NULL)
//FIXME    sender = g_background_thread_sender.Pointer()->get();
//FIXME
//FIXME  if (!sender->Send(new ChromeViewHostMsg_GetReadonlyPnaclFD(
//FIXME          std::string(filename),
//FIXME          &out_fd))) {
    return base::kInvalidPlatformFileValue;
//FIXME  }
//FIXME
//FIXME  if (out_fd == IPC::InvalidPlatformFileForTransit()) {
//FIXME    return base::kInvalidPlatformFileValue;
//FIXME  }
//FIXME
//FIXME  base::PlatformFile handle =
//FIXME      IPC::PlatformFileForTransitToPlatformFile(out_fd);
//FIXME  return handle;
}

PP_FileHandle CreateTemporaryFile(PP_Instance instance) {
    ASSERT_NOT_REACHED();
//FIXME  IPC::PlatformFileForTransit transit_fd = IPC::InvalidPlatformFileForTransit();
//FIXME  IPC::Sender* sender = content::RenderThread::Get();
//FIXME  if (sender == NULL)
//FIXME    sender = g_background_thread_sender.Pointer()->get();
//FIXME
//FIXME  if (!sender->Send(new ChromeViewHostMsg_NaClCreateTemporaryFile(
//FIXME          &transit_fd))) {
//FIXME    return base::kInvalidPlatformFileValue;
//FIXME  }
//FIXME
//FIXME  if (transit_fd == IPC::InvalidPlatformFileForTransit()) {
    return base::kInvalidPlatformFileValue;
//FIXME  }
//FIXME
//FIXME  base::PlatformFile handle = IPC::PlatformFileForTransitToPlatformFile(
//FIXME      transit_fd);
//FIXME  return handle;
}

PP_Bool IsOffTheRecord() {
  ASSERT_NOT_REACHED();
  return PP_FromBool(false); //FIXME ChromeRenderProcessObserver::is_incognito_process());
}

PP_Bool IsPnaclEnabled() {
  return PP_FromBool(false);
//FIXME      CommandLine::ForCurrentProcess()->HasSwitch(switches::kEnablePnacl));
}

PP_NaClResult ReportNaClError(PP_Instance instance,
                              PP_NaClError error_id) {
    ASSERT_NOT_REACHED();
//FIXME  IPC::Sender* sender = content::RenderThread::Get();
//FIXME
//FIXME  if (!sender->Send(
//FIXME          new ChromeViewHostMsg_NaClErrorStatus(
//FIXME              // TODO(dschuff): does this enum need to be sent as an int,
//FIXME              // or is it safe to include the appropriate headers in
//FIXME              // render_messages.h?
//FIXME              GetRoutingID(instance), static_cast<int>(error_id)))) {
//FIXME    return PP_NACL_FAILED;
//FIXME  }
  return PP_NACL_OK;
}

const PPB_NaCl_Private nacl_interface = {
  &LaunchSelLdr,
  &StartPpapiProxy,
  &UrandomFD,
  &Are3DInterfacesDisabled,
  &EnableBackgroundSelLdrLaunch,
  &BrokerDuplicateHandle,
  &GetReadonlyPnaclFD,
  &CreateTemporaryFile,
  &IsOffTheRecord,
  &IsPnaclEnabled,
  &ReportNaClError
};

}  // namespace

const PPB_NaCl_Private* PPB_NaCl_Private_Impl::GetInterface() {
  return &nacl_interface;
}

#endif  // DISABLE_NACL

