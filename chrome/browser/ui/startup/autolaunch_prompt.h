// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_UI_STARTUP_AUTOLAUNCH_PROMPT_H_
#define CHROME_BROWSER_UI_STARTUP_AUTOLAUNCH_PROMPT_H_

class PrefServiceSyncable;
class Browser;

namespace chrome {

// Determines whether or not the auto-launch prompt should be shown, and shows
// it as needed. Returns true if it was shown, false otherwise.
bool ShowAutolaunchPrompt(Browser* browser);

// Registers auto-launch specific prefs.
void RegisterAutolaunchUserPrefs(PrefServiceSyncable* prefs);

}  // namespace chrome

#endif  // CHROME_BROWSER_UI_STARTUP_AUTOLAUNCH_PROMPT_H_
