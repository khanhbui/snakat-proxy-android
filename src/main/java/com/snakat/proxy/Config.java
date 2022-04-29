package com.snakat.proxy;

import android.content.Context;
import android.content.res.Resources;

import androidx.annotation.BoolRes;
import androidx.annotation.NonNull;
import androidx.annotation.StringRes;

public class Config {

    private final String mDocumentDir;
    private final boolean mSslEnabled;
    private final String mHostName;
    private final String mSslCertFilename;
    private final String mManifestFilename;
    private final boolean mLogEnabled;

    private Config(Builder builder) {
        mDocumentDir = builder.mDocumentDir;
        mSslEnabled = builder.mSslEnabled;
        mHostName = builder.mHostName;
        mSslCertFilename = builder.mSslCertFilename;
        mManifestFilename = builder.mManifestFilename;
        mLogEnabled = builder.mLogEnabled;
    }

    public String getDocumentDir() {
        return mDocumentDir;
    }

    public boolean isSslEnabled() {
        return mSslEnabled;
    }

    public String getHostName() {
        return mHostName;
    }

    public String getSslCertFilename() {
        return mSslCertFilename;
    }

    public String getManifestFilename() {
        return mManifestFilename;
    }

    public boolean isLogEnabled() {
        return mLogEnabled;
    }

    public static class Builder {

        private final Resources mResources;

        private String mDocumentDir;
        private boolean mSslEnabled;
        private String mHostName;
        private String mSslCertFilename;
        private String mManifestFilename;
        private boolean mLogEnabled;

        public Builder(@NonNull Context context) {
            mResources = context.getResources();
        }

        public Builder setDocumentDir(String documentDir) {
            mDocumentDir = documentDir;
            return this;
        }

        public Builder setSslEnabled(boolean sslEnabled) {
            mSslEnabled = sslEnabled;
            return this;
        }

        public Builder setSslEnabled(@BoolRes int sslEnabled) {
            return setSslEnabled(mResources.getBoolean(sslEnabled));
        }

        public Builder setHostName(@NonNull String hostName) {
            mHostName = hostName;
            return this;
        }

        public Builder setHostName(@StringRes int hostname) {
            return setHostName(mResources.getString(hostname));
        }

        public Builder setSslCertFilename(@NonNull String sslCertFilename) {
            mSslCertFilename = sslCertFilename;
            return this;
        }

        public Builder setSslCertFilename(@StringRes int sslCertFilename) {
            return setSslCertFilename(mResources.getString(sslCertFilename));
        }

        public Builder setManifestFilename(@NonNull String manifestFilename) {
            mManifestFilename = manifestFilename;
            return this;
        }

        public Builder setManifestFilename(@StringRes int manifestFilename) {
            return setManifestFilename(mResources.getString(manifestFilename));
        }

        public Builder setLogEnabled(boolean logEnabled) {
            mLogEnabled = logEnabled;
            return this;
        }

        public Builder setLogEnabled(@BoolRes int logEnabled) {
            return setSslEnabled(mResources.getBoolean(logEnabled));
        }

        public Config build() {
            return new Config(this);
        }
    }
}
