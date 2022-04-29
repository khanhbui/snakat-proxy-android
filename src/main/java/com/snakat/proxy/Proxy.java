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

    public static void createInstance(@NonNull Context context, @NonNull Config config) {
        if (mInstance == null) {
            mInstance = new Proxy(context, config);
        }
    }

    public static void destroyInstance() {
        mInstance.dispose();
        mInstance = null;
    }

    private Proxy(Context context, @NonNull Config config) {
        super(context, config);
    }

    public void start() {
        startJNI();
    }

    public void pause() {
        pauseJNI();
    }

    public void resume() {
        resumeJNI();
    }

    public void stop() {
        stopJNI();
    }

    public boolean isStarted() {
        return isStartedJNI();
    }

    public boolean isRunning() {
        return isRunningJNI();
    }
}