package com.snakat.proxy.data;

import com.google.gson.annotations.SerializedName;

public class Manifest {
    @SerializedName("filename")
    private String mFilename;
    @SerializedName("hash")
    private String mHash;
    @SerializedName("size")
    private long mSize;

    public String getFilename() {
        return mFilename;
    }

    public String getHash() {
        return mHash;
    }

    public long getSize() {
        return mSize;
    }
}
