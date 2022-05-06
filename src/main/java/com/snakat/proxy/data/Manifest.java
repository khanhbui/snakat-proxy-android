package com.snakat.proxy.data;

import com.google.gson.annotations.SerializedName;

public class Manifest {
//
//    @SerializedName("list")
//    private List<Item> mList = new ArrayList<>();
//
//    public List<Item> getList() {
//        return mList;
//    }
//
//    public static long getTotalSize(List<Manifest> list) {
//        long total = 0;
//        for (Manifest item : list) {
//            total += item.mSize;
//        }
//        return total;
//    }

//    public static class Item {
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
//    }
}
