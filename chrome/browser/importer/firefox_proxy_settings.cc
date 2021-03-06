// Copyright (c) 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/importer/firefox_proxy_settings.h"

#include "base/file_path.h"
#include "base/string_util.h"
#include "base/strings/string_tokenizer.h"
#include "base/values.h"
#include "chrome/browser/importer/firefox_importer_utils.h"
#include "net/proxy/proxy_config.h"

namespace {

const char* const kNetworkProxyTypeKey = "network.proxy.type";
const char* const kHTTPProxyKey = "network.proxy.http";
const char* const kHTTPProxyPortKey = "network.proxy.http_port";
const char* const kSSLProxyKey = "network.proxy.ssl";
const char* const kSSLProxyPortKey = "network.proxy.ssl_port";
const char* const kFTPProxyKey = "network.proxy.ftp";
const char* const kFTPProxyPortKey = "network.proxy.ftp_port";
const char* const kGopherProxyKey = "network.proxy.gopher";
const char* const kGopherProxyPortKey = "network.proxy.gopher_port";
const char* const kSOCKSHostKey = "network.proxy.socks";
const char* const kSOCKSHostPortKey = "network.proxy.socks_port";
const char* const kSOCKSVersionKey = "network.proxy.socks_version";
const char* const kAutoconfigURL = "network.proxy.autoconfig_url";
const char* const kNoProxyListKey = "network.proxy.no_proxies_on";
const char* const kPrefFileName = "prefs.js";

FirefoxProxySettings::ProxyConfig IntToProxyConfig(int type) {
  switch (type) {
    case 1:
      return FirefoxProxySettings::MANUAL;
    case 2:
      return FirefoxProxySettings::AUTO_FROM_URL;
    case 4:
      return FirefoxProxySettings::AUTO_DETECT;
    case 5:
      return FirefoxProxySettings::SYSTEM;
    default:
      LOG(ERROR) << "Unknown Firefox proxy config type: " << type;
      return FirefoxProxySettings::NO_PROXY;
  }
}

FirefoxProxySettings::SOCKSVersion IntToSOCKSVersion(int type) {
  switch (type) {
    case 4:
      return FirefoxProxySettings::V4;
    case 5:
      return FirefoxProxySettings::V5;
    default:
      LOG(ERROR) << "Unknown Firefox proxy config type: " << type;
      return FirefoxProxySettings::UNKNONW;
  }
}

}  // namespace

FirefoxProxySettings::FirefoxProxySettings() {
  Reset();
}

FirefoxProxySettings::~FirefoxProxySettings() {
}

void FirefoxProxySettings::Reset() {
  config_type_ = NO_PROXY;
  http_proxy_.clear();
  http_proxy_port_ = 0;
  ssl_proxy_.clear();
  ssl_proxy_port_ = 0;
  ftp_proxy_.clear();
  ftp_proxy_port_ = 0;
  gopher_proxy_.clear();
  gopher_proxy_port_ = 0;
  socks_host_.clear();
  socks_port_ = 0;
  socks_version_ = UNKNONW;
  proxy_bypass_list_.clear();
  autoconfig_url_.clear();
}

// static
bool FirefoxProxySettings::GetSettings(FirefoxProxySettings* settings) {
  DCHECK(settings);
  settings->Reset();

  FilePath profile_path = GetFirefoxProfilePath();
  if (profile_path.empty())
    return false;
  FilePath pref_file = profile_path.AppendASCII(kPrefFileName);
  return GetSettingsFromFile(pref_file, settings);
}

bool FirefoxProxySettings::ToProxyConfig(net::ProxyConfig* config) {
  switch (config_type()) {
    case NO_PROXY:
      *config = net::ProxyConfig::CreateDirect();
      return true;
    case AUTO_DETECT:
      *config = net::ProxyConfig::CreateAutoDetect();
      return true;
    case AUTO_FROM_URL:
      *config = net::ProxyConfig::CreateFromCustomPacURL(
          GURL(autoconfig_url()));
      return true;
    case SYSTEM:
      // Can't convert this directly to a ProxyConfig.
      return false;
    case MANUAL:
      // Handled outside of the switch (since it is a lot of code.)
      break;
    default:
      NOTREACHED();
      return false;
  }

  // The rest of this funciton is for handling the MANUAL case.
  DCHECK_EQ(MANUAL, config_type());

  *config = net::ProxyConfig();
  config->proxy_rules().type =
      net::ProxyConfig::ProxyRules::TYPE_PROXY_PER_SCHEME;

  if (!http_proxy().empty()) {
    config->proxy_rules().proxy_for_http = net::ProxyServer(
        net::ProxyServer::SCHEME_HTTP,
        net::HostPortPair(http_proxy(), http_proxy_port()));
  }

  if (!ftp_proxy().empty()) {
    config->proxy_rules().proxy_for_ftp = net::ProxyServer(
        net::ProxyServer::SCHEME_HTTP,
        net::HostPortPair(ftp_proxy(), ftp_proxy_port()));
  }

  if (!ssl_proxy().empty()) {
    config->proxy_rules().proxy_for_https = net::ProxyServer(
        net::ProxyServer::SCHEME_HTTP,
        net::HostPortPair(ssl_proxy(), ssl_proxy_port()));
  }

  if (!socks_host().empty()) {
    net::ProxyServer::Scheme proxy_scheme = V5 == socks_version() ?
        net::ProxyServer::SCHEME_SOCKS5 : net::ProxyServer::SCHEME_SOCKS4;

    config->proxy_rules().fallback_proxy = net::ProxyServer(
        proxy_scheme,
        net::HostPortPair(socks_host(), socks_port()));
  }

  config->proxy_rules().bypass_rules.ParseFromStringUsingSuffixMatching(
      JoinString(proxy_bypass_list_, ';'));

  return true;
}

// static
bool FirefoxProxySettings::GetSettingsFromFile(const FilePath& pref_file,
                                               FirefoxProxySettings* settings) {
  DictionaryValue dictionary;
  if (!ParsePrefFile(pref_file, &dictionary))
    return false;

  int proxy_type = 0;
  if (!dictionary.GetInteger(kNetworkProxyTypeKey, &proxy_type))
    return true;  // No type means no proxy.

  settings->config_type_ = IntToProxyConfig(proxy_type);
  if (settings->config_type_ == AUTO_FROM_URL) {
    if (!dictionary.GetStringASCII(kAutoconfigURL,
                                   &(settings->autoconfig_url_))) {
      LOG(ERROR) << "Failed to retrieve Firefox proxy autoconfig URL";
    }
    return true;
  }

  if (settings->config_type_ == MANUAL) {
    if (!dictionary.GetStringASCII(kHTTPProxyKey, &(settings->http_proxy_)))
      LOG(ERROR) << "Failed to retrieve Firefox proxy HTTP host";
    if (!dictionary.GetInteger(kHTTPProxyPortKey,
                               &(settings->http_proxy_port_))) {
      LOG(ERROR) << "Failed to retrieve Firefox proxy HTTP port";
    }
    if (!dictionary.GetStringASCII(kSSLProxyKey, &(settings->ssl_proxy_)))
      LOG(ERROR) << "Failed to retrieve Firefox proxy SSL host";
    if (!dictionary.GetInteger(kSSLProxyPortKey, &(settings->ssl_proxy_port_)))
      LOG(ERROR) << "Failed to retrieve Firefox proxy SSL port";
    if (!dictionary.GetStringASCII(kFTPProxyKey, &(settings->ftp_proxy_)))
      LOG(ERROR) << "Failed to retrieve Firefox proxy FTP host";
    if (!dictionary.GetInteger(kFTPProxyPortKey, &(settings->ftp_proxy_port_)))
      LOG(ERROR) << "Failed to retrieve Firefox proxy SSL port";
    if (!dictionary.GetStringASCII(kGopherProxyKey, &(settings->gopher_proxy_)))
      LOG(ERROR) << "Failed to retrieve Firefox proxy gopher host";
    if (!dictionary.GetInteger(kGopherProxyPortKey,
                               &(settings->gopher_proxy_port_))) {
      LOG(ERROR) << "Failed to retrieve Firefox proxy gopher port";
    }
    if (!dictionary.GetStringASCII(kSOCKSHostKey, &(settings->socks_host_)))
      LOG(ERROR) << "Failed to retrieve Firefox SOCKS host";
    if (!dictionary.GetInteger(kSOCKSHostPortKey, &(settings->socks_port_)))
      LOG(ERROR) << "Failed to retrieve Firefox SOCKS port";
    int socks_version;
    if (dictionary.GetInteger(kSOCKSVersionKey, &socks_version))
      settings->socks_version_ = IntToSOCKSVersion(socks_version);

    std::string proxy_bypass;
    if (dictionary.GetStringASCII(kNoProxyListKey, &proxy_bypass) &&
        !proxy_bypass.empty()) {
      base::StringTokenizer string_tok(proxy_bypass, ",");
      while (string_tok.GetNext()) {
        std::string token = string_tok.token();
        TrimWhitespaceASCII(token, TRIM_ALL, &token);
        if (!token.empty())
          settings->proxy_bypass_list_.push_back(token);
      }
    }
  }
  return true;
}
