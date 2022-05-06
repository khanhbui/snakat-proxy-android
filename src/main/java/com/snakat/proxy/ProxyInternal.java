package com.snakat.proxy;

import android.content.Context;
import android.content.res.AssetManager;
import android.util.Log;

import androidx.annotation.NonNull;

import com.google.gson.Gson;
import com.snakat.proxy.data.Manifest;
import com.snakat.proxy.data.ManifestList;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.lang.ref.WeakReference;
import java.util.Objects;
import java.util.concurrent.Callable;

import io.reactivex.Completable;
import io.reactivex.CompletableSource;
import io.reactivex.Maybe;
import io.reactivex.MaybeSource;
import io.reactivex.android.schedulers.AndroidSchedulers;
import io.reactivex.functions.Action;
import io.reactivex.functions.Consumer;
import io.reactivex.functions.Function;
import io.reactivex.schedulers.Schedulers;
import okhttp3.ResponseBody;
import retrofit2.Retrofit;
import retrofit2.adapter.rxjava2.RxJava2CallAdapterFactory;
import retrofit2.converter.gson.GsonConverterFactory;

abstract class ProxyInternal {

    protected static final String TAG = Proxy.class.getName();
    private static final int PARALLEL_DOWNLOAD_NUM = 2;

    static boolean LOG_ENABLED = false;

    protected final WeakReference<Context> mContext;
    protected final Config mConfig;
    private final ManifestService mManifestService;

    private ManifestList mRemoteManifestList;

    protected ProxyInternal(@NonNull Context context, @NonNull Config config) {
        mContext = new WeakReference<>(context);
        mConfig = config;

        LOG_ENABLED = config.isLogEnabled();

        initJNI(context.getAssets(),
                config.getDocumentDir(),
                config.getHostName(),
                config.isSslEnabled(),
                config.getSslCertFilename());

        Retrofit retrofit = new Retrofit.Builder()
                .baseUrl(mConfig.getRemoteUrl())
                .addConverterFactory(GsonConverterFactory.create())
                .addCallAdapterFactory(RxJava2CallAdapterFactory.create())
                .build();
        mManifestService = retrofit.create(ManifestService.class);
    }

    protected void dispose() {
        stop();
        disposeJNI();

        mContext.clear();
    }

    boolean deleteRecursive(@NonNull File path) {
        if (!path.exists()) {
            return false;
        }
        if (path.isDirectory()) {
            for (File file : Objects.requireNonNull(path.listFiles())) {
                if (!deleteRecursive(file)) {
                    return false;
                }
            }
        }
        return path.delete();
    }

    @NonNull
    protected Maybe<ManifestList> getRemoteManifest() {
        return mManifestService
                .getManifest(mConfig.getManifestFilename())
                .doOnSuccess(new Consumer<ManifestList>() {
                    @Override
                    public void accept(ManifestList manifestList) throws Exception {
                        mRemoteManifestList = manifestList;
                    }
                })
                .subscribeOn(Schedulers.io());
    }

    @NonNull
    protected Completable saveRemoteManifest() {
        return Completable.defer(new Callable<CompletableSource>() {
            @Override
            public CompletableSource call() throws Exception {
                String json = new Gson().toJson(mRemoteManifestList);
                if (json != null) {
                    String manifestFilename = mConfig.getManifestFilename();
                    File cachedManifest = new File(mConfig.getDocumentDir(), manifestFilename);
                    if (!cachedManifest.exists()) {
                        cachedManifest.createNewFile();
                    }

                    FileOutputStream outputStream = new FileOutputStream(cachedManifest);
                    OutputStreamWriter outputStreamWriter = new OutputStreamWriter(outputStream);
                    outputStreamWriter.append(json);
                    outputStreamWriter.close();
                    outputStream.flush();
                    outputStream.close();
                }
                return Completable.complete();
            }
        });
    }

    @NonNull
    protected Maybe<ManifestList> getLocalManifest() {
        return Maybe.defer(new Callable<MaybeSource<? extends ManifestList>>() {
            @Override
            public MaybeSource<? extends ManifestList> call() throws Exception {
                String manifestFilename = mConfig.getManifestFilename();
                File cachedManifest = new File(mConfig.getDocumentDir(), manifestFilename);
                InputStream input;
                if (cachedManifest.exists()) {
                    input = new FileInputStream(cachedManifest);
                } else {
                    input = mContext.get().getAssets().open(manifestFilename);
                }
                InputStreamReader reader = new InputStreamReader(input);
                ManifestList manifestList = new Gson().fromJson(reader, ManifestList.class);
                input.close();
                reader.close();
                return Maybe.just(manifestList);
            }
        }).subscribeOn(Schedulers.io());
    }

    @NonNull
    protected Completable downloadFiles(@NonNull ManifestList manifestList, @NonNull DownloadListener downloadListener) {
        int size = manifestList.size();
        if (size == 0) {
            return Completable.complete();
        }

        Completable completable = downloadFile(manifestList.get(0), downloadListener);
        for (int i = 1; i < size;) {
            Log.i(TAG, "i=" + i + ", size=" + size);

            int next = i + PARALLEL_DOWNLOAD_NUM;
            Completable parallelDownload = downloadFile(manifestList.get(i), downloadListener);
            for (i++; i < next && i < size; i++) {
                Log.i(TAG, "---i=" + i + ", size=" + size + ", next=" + next);
                parallelDownload = parallelDownload.mergeWith(downloadFile(manifestList.get(i), downloadListener));
            }
            completable = completable.andThen(parallelDownload);
        }
        return completable;
    }

    @NonNull
    private Completable downloadFile(@NonNull Manifest manifest, @NonNull DownloadListener downloadListener) {
        return downloadFile(manifest.getFilename())
                .observeOn(AndroidSchedulers.mainThread())
                .doOnComplete(new Action() {
                    @Override
                    public void run() throws Exception {
                        downloadListener.onProgress(manifest);
                    }
                });
    }

    @NonNull
    private Completable downloadFile(@NonNull String filename) {
        return mManifestService.downloadFile(filename)
                .flatMapCompletable(new Function<ResponseBody, CompletableSource>() {
                    @Override
                    public CompletableSource apply(ResponseBody responseBody) throws Exception {
                        File file = new File(mConfig.getDocumentDir(), filename);
                        if (!file.exists()) {
                            File basePath = file.getParentFile();
                            if (basePath != null && !basePath.exists()) {
                                basePath.mkdirs();
                            }
                            file.createNewFile();
                        }

                        InputStream inputStream = responseBody.byteStream();
                        FileOutputStream outputStream = new FileOutputStream(file);

                        byte[] buffer = new byte[4096];
                        int len;
                        while ((len = inputStream.read(buffer)) != -1) {
                            outputStream.write(buffer, 0, len);
                        }
                        outputStream.flush();
                        outputStream.close();

                        return Completable.complete();
                    }
                })
                .subscribeOn(Schedulers.io());
    }



    protected native void initJNI(AssetManager assetManager, String documentDir, String hostName, boolean sslEnabled, String sslCertFilename);

    protected native void startJNI();

    protected native void pauseJNI();

    protected native void resumeJNI();

    protected native void stopJNI();

    protected native void disposeJNI();

    protected native boolean isStartedJNI();

    protected native boolean isRunningJNI();

    public abstract void stop();
}
