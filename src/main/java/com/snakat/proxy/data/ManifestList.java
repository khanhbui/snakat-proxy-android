package com.snakat.proxy.data;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import java.util.ArrayList;

public class ManifestList extends ArrayList<Manifest> {

    @Nullable
    public Manifest get(@NonNull String filename) {
        for (Manifest item : this) {
            if (item.getFilename().equals(filename)) {
                return item;
            }
        }
        return null;
    }

    public boolean contains(@NonNull Manifest manifest) {
        for (Manifest item : this) {
            if (item.getSize() == manifest.getSize()
                    && item.getHash().equals(manifest.getHash())
                    && item.getFilename().equals(manifest.getFilename())) {
                return true;
            }
        }
        return false;
    }

    public long getTotalSize() {
        long total = 0;
        for (Manifest item : this) {
            total += item.getSize();
        }
        return total;
    }

    public double getTotalSizeMB() {
        return (double) getTotalSize() / (1024 * 1024);
    }
}
