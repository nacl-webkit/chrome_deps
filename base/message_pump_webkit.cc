// Copyright (c) 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/message_pump_webkit.h"
#include "base/logging.h"
#include "base/time.h"

namespace base {

MessagePumpWebKit::MessagePumpWebKit(MessagePumpWebKitHelper* helper) {
    DCHECK(helper);
    helper_ = helper;
    should_quit_ = false;
}

MessagePumpWebKit::~MessagePumpWebKit() {
}

void MessagePumpWebKit::Run(Delegate* delegate)
{
    DCHECK(helper_);
    helper_->Run(delegate);
}

void MessagePumpWebKit::Quit()
{
    DCHECK(helper_);
    should_quit_ = true;
    helper_->Quit();
}

void MessagePumpWebKit::ScheduleWork()
{
    DCHECK(helper_);
    helper_->ScheduleWork();
}

void MessagePumpWebKit::ScheduleDelayedWork(const TimeTicks& delayed_work_time)
{
    DCHECK(helper_);
    helper_->ScheduleDelayedWork(delayed_work_time);
}

void MessagePumpWebKit::RunWithDispatcher(Delegate* delegate, MessagePumpDispatcher* dispatcher)
{
    DCHECK(helper_);
    helper_->RunWithDispatcher(delegate, dispatcher);
}

void MessagePumpWebKit::ScheduleWorkCallback(TimeTicks& delayed_work_time)
{
    bool didWork = false;
    if (delegate_->DoWork())
        didWork = true;
    DCHECK(!should_quit_);
    if (delegate_->DoDelayedWork(&delayed_work_time))
        didWork = true;
    DCHECK(!should_quit_);
    if (!didWork && delegate_->DoIdleWork())
        didWork = true;
    if (didWork)
        ScheduleDelayedWork(delayed_work_time);
}

}  // namespace base
