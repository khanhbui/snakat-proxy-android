package com.snakat.proxy;

import android.content.Context;

import java.lang.ref.WeakReference;

class ProxyInternal {

    private static final String TAG = Proxy.class.getName();

    static boolean LOG_ENABLED = false;

    protected final WeakReference<Context> mContext;

    protected boolean mInitialized = false;

    protected ProxyInternal(Context context, boolean logEnabled) {
        mContext = new WeakReference<>(context);

        LOG_ENABLED = logEnabled;
    }

    protected void dispose() {
        mContext.clear();
    }
}
