// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NET_QUIC_QUIC_CLOCK_H_
#define NET_QUIC_QUIC_CLOCK_H_

#include "base/basictypes.h"
#include "net/base/net_export.h"
#include "net/quic/quic_time.h"

namespace net {

typedef double WallTime;

// Clock to efficiently retrieve an approximately accurate time from an
// EpollServer.
class NET_EXPORT_PRIVATE QuicClock {
 public:
  QuicClock();
  virtual ~QuicClock();

  // Returns the approximate current time as a QuicTime object.
  virtual QuicTime Now() const;

  // Returns the current time as an offset from the Unix epoch (1970-01-01
  // 00:00:00 GMT). This function may return a smaller Delta in subsequent
  // calls if the system clock is changed.
  virtual QuicTime::Delta NowAsDeltaSinceUnixEpoch() const;
};

}  // namespace net

#endif  // NET_QUIC_QUIC_CLOCK_H_
