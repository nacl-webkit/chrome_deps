// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_MESSAGE_PUMP_WEBKIT_H_
#define BASE_MESSAGE_PUMP_WEBKIT_H_

#include "base/message_pump.h"
#include "base/message_pump_dispatcher.h"
#include "base/message_pump_observer.h"

namespace base {

class TimeTicks;

class BASE_EXPORT MessagePumpWebKit : public MessagePump {
 public:
  class MessagePumpWebKitHelper {
   public:
    virtual void Run(Delegate* delegate) = 0;
    virtual void Quit() = 0;
    virtual void ScheduleWork() = 0;
    virtual void ScheduleDelayedWork(const TimeTicks& delayed_work_time) = 0;
    virtual void RunWithDispatcher(Delegate* delegate,
                                   MessagePumpDispatcher* dispatcher) = 0;
    virtual ~MessagePumpWebKitHelper() {}
  };

  MessagePumpWebKit(MessagePumpWebKitHelper* = 0);

  // MessagePump methods:
  virtual void Run(Delegate* delegate) OVERRIDE;
  virtual void Quit() OVERRIDE;
  virtual void ScheduleWork() OVERRIDE;
  virtual void ScheduleDelayedWork(const TimeTicks& delayed_work_time) OVERRIDE;
  virtual void RunWithDispatcher(Delegate* delegate,
                                 MessagePumpDispatcher* dispatcher);
  void setDelegate(Delegate* delegate) {delegate_ = delegate; }
  void ScheduleWorkCallback(TimeTicks& delayed_work_time);

 protected:
  virtual ~MessagePumpWebKit();
  MessagePumpWebKitHelper* helper_;
  Delegate* delegate_;
  bool should_quit_;
};

typedef MessagePumpWebKit MessagePumpForUI;

}  // namespace base

#endif  // BASE_MESSAGE_PUMP_WEBKIT_H_
