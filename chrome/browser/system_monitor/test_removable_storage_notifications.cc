// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/system_monitor/test_removable_storage_notifications.h"

namespace chrome {
namespace test {

TestRemovableStorageNotifications::TestRemovableStorageNotifications()
    : RemovableStorageNotifications() {}

TestRemovableStorageNotifications::~TestRemovableStorageNotifications() {}

bool TestRemovableStorageNotifications::GetDeviceInfoForPath(
    const FilePath& path,
    StorageInfo* device_info) const {
  return false;
}

uint64 TestRemovableStorageNotifications::GetStorageSize(
    const std::string& location) const {
  return 0;
}

#if defined(OS_WIN)
bool TestRemovableStorageNotifications::GetMTPStorageInfoFromDeviceId(
    const std::string& storage_device_id,
    string16* device_location,
    string16* storage_object_id) const {
  return false;
}
#endif

}  // namespace test
}  // namespace chrome
