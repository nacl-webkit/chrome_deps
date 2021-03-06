// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// TransientDeviceIds keep track of transient IDs for removable devices, so
// persistent device IDs are not exposed to renderers. Once a removable device
// gets mapped to a transient ID, the mapping remains valid for the duration of
// TransientDeviceIds' lifetime.

#ifndef CHROME_BROWSER_MEDIA_GALLERY_TRANSIENT_DEVICE_IDS_H_
#define CHROME_BROWSER_MEDIA_GALLERY_TRANSIENT_DEVICE_IDS_H_

#include <map>
#include <string>

#include "base/basictypes.h"
#include "base/threading/thread_checker.h"

namespace chrome {

class TransientDeviceIds {
 public:
  TransientDeviceIds();
  ~TransientDeviceIds();

  // Returns the transient ID for a given |device_id|.
  // |device_id| must be for a removable device.
  // If |device_id| has never been seen before, a new, unique transient id will
  // be assigned.
  uint64 GetTransientIdForDeviceId(const std::string& device_id);

 private:
  typedef std::map<std::string, uint64> DeviceIdToTransientIdMap;

  DeviceIdToTransientIdMap id_map_;
  uint64 next_transient_id_;

  base::ThreadChecker thread_checker_;

  DISALLOW_COPY_AND_ASSIGN(TransientDeviceIds);
};

}  // namespace chrome

#endif  // CHROME_BROWSER_MEDIA_GALLERY_TRANSIENT_DEVICE_IDS_H_
