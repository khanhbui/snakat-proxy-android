package com.snakat.proxy;

import androidx.annotation.NonNull;

import com.snakat.proxy.data.Manifest;
import com.snakat.proxy.data.ManifestList;

public interface DownloadListener {

    void onConfirm(@NonNull ManifestList manifestList, final @NonNull Executor executor);

    void onStart();

    void onCancel();

    void onError(Throwable t);

    void onProgress(@NonNull Manifest manifest);

    void onDone();
}
