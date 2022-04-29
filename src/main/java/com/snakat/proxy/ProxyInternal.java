package com.snakat.proxy;

import android.content.Context;
import android.content.res.AssetManager;

import androidx.annotation.NonNull;

import java.lang.ref.WeakReference;

abstract class ProxyInternal {

    private static final String TAG = Proxy.class.getName();

    static boolean LOG_ENABLED = false;

    protected final WeakReference<Context> mContext;
    protected final Config mConfig;

    protected boolean mInitialized = false;

    protected ProxyInternal(Context context, @NonNull Config config) {
        mContext = new WeakReference<>(context);
        mConfig = config;

        LOG_ENABLED = config.isLogEnabled();

        initJNI(context.getAssets(),
                config.getDocumentDir(),
                config.getHostName(),
                config.isSslEnabled(),
                config.getSslCertFilename(),
                config.getManifestFilename());
    }

    protected void dispose() {
        stop();
        disposeJNI();

        mContext.clear();
    }

    protected native void initJNI(AssetManager assetManager, String documentDir, String hostName, boolean sslEnabled, String sslCertFilename, String manifestFilename);

    protected native void startJNI();

    protected native void pauseJNI();

    protected native void resumeJNI();

    protected native void stopJNI();

    protected native void disposeJNI();

    protected native boolean isStartedJNI();

    protected native boolean isRunningJNI();

    public abstract void stop();
}
