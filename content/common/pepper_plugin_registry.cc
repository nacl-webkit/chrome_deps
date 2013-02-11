// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "content/common/pepper_plugin_registry.h"

#include "Logging.h"
#include "Module.h"
#include "webkit/plugins/ppapi/plugin_module.h"
#include "ProcessExecutablePath.h"
#include "content/public/common/content_switches.h"
#include <WebCore/FileSystem.h>
#include <wtf/Assertions.h>
#include <wtf/text/CString.h>
#include <wtf/text/WTFString.h>
#include "ppapi/shared_impl/ppapi_permissions.h"
#include "base/command_line.h"
#include "base/file_util.h"
#include "base/native_library.h"
#include "base/string_split.h"
#include "base/string_util.h"
#include "base/utf_string_conversions.h"
#include "content/public/common/content_client.h"
#include "content/public/common/content_switches.h"
#include "ppapi/shared_impl/ppapi_permissions.h"
#include "webkit/plugins/npapi/plugin_list.h"

using namespace WebCore;
using namespace WebKit;

namespace content {
namespace {

// Appends any plugins from the command line to the given vector.
void ComputePluginsFromCommandLine(std::vector<PepperPluginInfo>* plugins) {
  bool out_of_process =
      CommandLine::ForCurrentProcess()->HasSwitch(switches::kPpapiOutOfProcess);
  const std::string value =
      CommandLine::ForCurrentProcess()->GetSwitchValueASCII(
          switches::kRegisterPepperPlugins);
  if (value.empty())
    return;

  // FORMAT:
  // command-line = <plugin-entry> + *( LWS + "," + LWS + <plugin-entry> )
  // plugin-entry =
  //    <file-path> +
  //    ["#" + <name> + ["#" + <description> + ["#" + <version>]]] +
  //    *1( LWS + ";" + LWS + <mime-type> )
  std::vector<std::string> modules;
  base::SplitString(value, ',', &modules);
  for (size_t i = 0; i < modules.size(); ++i) {
    std::vector<std::string> parts;
    base::SplitString(modules[i], ';', &parts);
    if (parts.size() < 2) {
      //FIXME DLOG(ERROR) << "Required mime-type not found";
      continue;
    }

    std::vector<std::string> name_parts;
    base::SplitString(parts[0], '#', &name_parts);

    PepperPluginInfo plugin;
    plugin.type = out_of_process ? PepperPluginInfo::PLUGIN_TYPE_PEPPER_OUT_OF_PROCESS : PepperPluginInfo::PLUGIN_TYPE_PEPPER_IN_PROCESS;
#if defined(OS_WIN)
    // This means we can't provide plugins from non-ASCII paths, but
    // since this switch is only for development I don't think that's
    // too awful.
    plugin.path = std::string(ASCIIToUTF16(name_parts[0]));
#else
    plugin.path = std::string(name_parts[0]).c_str();
#endif
    if (name_parts.size() > 1)
    plugin.name = name_parts[1].c_str();
    if (name_parts.size() > 2)
    plugin.desc = name_parts[2].c_str();
    if (name_parts.size() > 3)
    plugin.version = name_parts[3].c_str();
    for (size_t j = 1; j < parts.size(); ++j) {
      PepperPluginMimeType mime_type(parts[j].c_str(), WTF::String(), plugin.desc);
      plugin.mime_types.push_back(mime_type);
    }

    // If the plugin name is empty, use the filename.
    if (plugin.name.isEmpty()) {
        ::FilePath filePath(std::string(name_parts[0].c_str()));
        plugin.name = UTF16ToUTF8(filePath.BaseName().LossyDisplayName()).c_str();
    }

    // Command-line plugins get full permissions.
    plugin.pepperPermissions = ::ppapi::PERMISSION_ALL_BITS;

    plugins->push_back(plugin);
  }
}

}  // namespace

// From chrome_content_client.cc
const char kNaClPluginName[] = "Native Client";
const char kNaClPluginMimeType[] = "application/x-nacl";
const char kNaClPluginExtension[] = "nexe";
const char kNaClPluginDescription[] = "Native Client Executable";
const uint32 kNaClPluginPermissions = ::ppapi::PERMISSION_PRIVATE | ::ppapi::PERMISSION_DEV;
const char kInternalNaClPluginFileName[] = "lib/libppWebKitNaClPlugin.so";

void PepperPluginRegistry::computeBuiltInPlugins(std::vector<PluginModuleInfo>& plugins)
{
    // FIXME: any other built-in plugins?
    String path = executablePathOfWebProcess();
    size_t binIndex = path.reverseFind("bin/");
    if (binIndex != notFound)
        path = path.substring(0, binIndex) + kInternalNaClPluginFileName; // FIXME: what is the path?
    else
        path = "";
    if (!path.isEmpty()) {
        if (fileExists(path)) {
            PluginModuleInfo nacl;
            nacl.path = path;
            nacl.info.name = kNaClPluginName;
            nacl.info.file = pathGetFileName(path);
            nacl.info.desc = kNaClPluginDescription;
            MimeClassInfo mimeInfo;
            mimeInfo.type = kNaClPluginMimeType;
            mimeInfo.desc = kNaClPluginDescription;
            mimeInfo.extensions.append(kNaClPluginExtension);
            nacl.info.mimes.append(mimeInfo);
            nacl.type = PepperPluginInfo::PLUGIN_TYPE_PEPPER_IN_PROCESS;
            nacl.pepperPermissions = kNaClPluginPermissions;
            plugins.push_back(nacl);
        }
    }
}

void PepperPluginRegistry::addPepperPlugins(std::vector<PepperPluginInfo>* plugins)
{
    std::vector<PluginModuleInfo> pluginModuleInfo;
    computeBuiltInPlugins(pluginModuleInfo);
    std::vector<PluginModuleInfo>::iterator end = pluginModuleInfo.end();
    for (std::vector<PluginModuleInfo>::iterator it = pluginModuleInfo.begin(); it != end; ++it)
        plugins->push_back(*it);
}

// static
PepperPluginRegistry* PepperPluginRegistry::GetInstance() {
  static PepperPluginRegistry* registry = NULL;
  // This object leaks.  It is a temporary hack to work around a crash.
  // http://code.google.com/p/chromium/issues/detail?id=63234
  if (!registry)
    registry = new PepperPluginRegistry;
  return registry;
}

// static
void PepperPluginRegistry::ComputeList(std::vector<PepperPluginInfo>* plugins) {
  GetContentClient()->AddPepperPlugins(plugins);
  ComputePluginsFromCommandLine(plugins);
}
/* FIXME
// static
void PepperPluginRegistry::PreloadModules() {
  std::vector<PepperPluginInfo> plugins;
  ComputeList(&plugins);
  for (size_t i = 0; i < plugins.size(); ++i) {
    if (!plugins[i].is_internal && plugins[i].is_sandboxed) {
      std::string error;
      base::NativeLibrary library = base::LoadNativeLibrary(plugins[i].path,
                                                            &error);
      DLOG_IF(WARNING, !library) << "Unable to load plugin "
                                 << plugins[i].path.value() << " "
                                 << error;
      (void)library;  // Prevent release-mode warning.
    }
  }
}
*/

void PepperPluginRegistry::computeList(std::vector<PluginModuleInfo>& plugins)
{
    std::vector<PepperPluginInfo> pluginInfos;
    computeList(&pluginInfos);

    for (int i = 0; i < pluginInfos.size(); i++){
        PluginModuleInfo info;
        pluginInfos[i].copyToPluginModuleInfo(info);
        plugins.push_back(info);
    }
}

  // We did not find the plugin in our list. But wait! the plugin can also
  // be a latecomer, as it happens with pepper flash. This information
  // is actually in |info| and we can use it to construct it and add it to
  // the list. This same deal needs to be done in the browser side in
  // PluginService.
    if (!isPepperPlugin(info))
        return NULL;

  plugin_list_.push_back(info);
  return &plugin_list_[plugin_list_.size() - 1];
}

webkit::ppapi::PluginModule* PepperPluginRegistry::GetLiveModule(
    const WTF::String& path) {
  NonOwningModuleMap::iterator it = live_modules_.find(path);
  if (it == live_modules_.end())
    return NULL;
  return it->second;
}

void PepperPluginRegistry::AddLiveModule(const WTF::String& path,
                                         webkit::ppapi::PluginModule* module) {
  DCHECK(live_modules_.find(path) == live_modules_.end());
  live_modules_[path] = module;
}

void PepperPluginRegistry::PluginModuleDead(
    webkit::ppapi::PluginModule* dead_module) {
  // DANGER: Don't dereference the dead_module pointer! It may be in the
  // process of being deleted.

  // Modules aren't destroyed very often and there are normally at most a
  // couple of them. So for now we just do a brute-force search.
  for (NonOwningModuleMap::iterator i = live_modules_.begin();
       i != live_modules_.end(); ++i) {
    if (i->second == dead_module) {
      live_modules_.erase(i);
      return;
    }
  }
  NOTREACHED();  // Should have always found the module above.
}

PepperPluginRegistry::~PepperPluginRegistry() {
  // Explicitly clear all preloaded modules first. This will cause callbacks
  // to erase these modules from the live_modules_ list, and we don't want
  // that to happen implicitly out-of-order.
  preloaded_modules_.clear();

  DCHECK(live_modules_.empty());
}

PepperPluginRegistry::PepperPluginRegistry() {
  ComputeList(&plugin_list_);

  // Note that in each case, AddLiveModule must be called before completing
  // initialization. If we bail out (in the continue clauses) before saving
  // the initialized module, it will still try to unregister itself in its
  // destructor.
  for (size_t i = 0; i < plugin_list_.size(); i++) {
    const PepperPluginInfo& current = plugin_list_[i];
    if (isOutOfProcessPlugin(current))
      continue;  // Out of process plugins need no special pre-initialization.

    scoped_refptr<webkit::ppapi::PluginModule> module =
        new webkit::ppapi::PluginModule(current.name, current.path, this,
            ppapi::PpapiPermissions(current.permissions));
    AddLiveModule(current.path, module);
    if (current.is_internal) {
      if (!module->InitAsInternalPlugin(current.internal_entry_points)) {
        DLOG(ERROR) << "Failed to load pepper module: " << current.path.value();
        continue;
      }
    } else {
      // Preload all external plugins we're not running out of process.
      if (!module->InitAsLibrary(current.path)) {
        DLOG(ERROR) << "Failed to load pepper module: " << current.path.value();
        continue;
      }
    }
    preloaded_modules_[current.path] = module;
  }
}

}  // namespace content
