// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package org.chromium.content_browsertests_apk;

import android.app.Activity;
import android.content.Context;
import android.os.Bundle;
import android.util.Log;

import org.chromium.content.app.LibraryLoader;
import org.chromium.content.browser.AndroidBrowserProcess;
import org.chromium.content.common.ProcessInitException;
import org.chromium.ui.gfx.ActivityNativeWindow;
import org.chromium.content_shell.ShellManager;

public class ContentBrowserTestsActivity extends Activity {
    private static final String TAG = "ChromeBrowserTestsActivity";

    private ShellManager mShellManager;
    private ActivityNativeWindow mActivityNativeWindow;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        try {
            LibraryLoader.ensureInitialized();
        } catch (ProcessInitException e) {
            Log.i(TAG, "Cannot load content_browsertests:" +  e);
        }
        AndroidBrowserProcess.initChromiumBrowserProcessForTests();

        setContentView(R.layout.test_activity);
        mShellManager = (ShellManager) findViewById(R.id.shell_container);
        mActivityNativeWindow = new ActivityNativeWindow(this);
        mShellManager.setWindow(mActivityNativeWindow);

        runTests();
        Log.i(TAG, "Tests finished.");
        finish();
    }

    private void runTests() {
        nativeRunTests(getFilesDir().getAbsolutePath(), getApplicationContext());
    }

    private native void nativeRunTests(String filesDir, Context appContext);
}
