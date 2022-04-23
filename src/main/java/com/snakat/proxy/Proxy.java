package com.snakat.proxy;

import android.content.Context;

import androidx.annotation.NonNull;

public final class Proxy extends ProxyInternal {

    // Used to load the 'proxy' library on application startup.
    static {
        System.loadLibrary("proxy");
    }

    private static Proxy mInstance;

    public static Proxy getInstance() {
        return mInstance;
    }

    public static void createInstance(@NonNull Context context) {
        createInstance(context, false);
    }

    public static void createInstance(@NonNull Context context, boolean logEnabled) {
        if (mInstance == null) {
            mInstance = new Proxy(context, logEnabled);
        }
    }

    public static void destroyInstance() {
        mInstance.dispose();
        mInstance = null;
    }

    protected Proxy(Context context, boolean logEnabled) {
        super(context, logEnabled);
    }

    /**
     * A native method that is implemented by the 'proxy' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();
}