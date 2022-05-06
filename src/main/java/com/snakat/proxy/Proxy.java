package com.snakat.proxy;

import android.content.Context;
import android.util.Log;

import androidx.annotation.NonNull;

import com.snakat.proxy.data.Manifest;
import com.snakat.proxy.data.ManifestList;

import java.io.File;
import java.util.concurrent.TimeUnit;

import io.reactivex.CompletableSource;
import io.reactivex.Maybe;
import io.reactivex.MaybeEmitter;
import io.reactivex.MaybeOnSubscribe;
import io.reactivex.MaybeSource;
import io.reactivex.android.schedulers.AndroidSchedulers;
import io.reactivex.disposables.Disposable;
import io.reactivex.functions.Action;
import io.reactivex.functions.BiFunction;
import io.reactivex.functions.Consumer;
import io.reactivex.functions.Function;
import io.reactivex.schedulers.Schedulers;

public final class Proxy extends ProxyInternal {

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

    private Disposable mDisposable;

    private Proxy(@NonNull Context context, @NonNull Config config) {
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

    public void downloadBundle(DownloadListener downloadListener) {
        downloadBundle(downloadListener, 0);
    }

    public void downloadBundle(DownloadListener downloadListener, long timeOut) {
        if (mDisposable != null) {
            mDisposable.dispose();
            mDisposable = null;
        }

        Maybe<ManifestList> getRemoteManifest = timeOut <= 0 ? getRemoteManifest() :
                getRemoteManifest().timeout(timeOut, TimeUnit.MILLISECONDS);

        mDisposable = Maybe
                .zip(getRemoteManifest,
                        getLocalManifest(),
                        new BiFunction<ManifestList, ManifestList, ManifestList>() {
                            @Override
                            public ManifestList apply(ManifestList remoteList, ManifestList localList) throws Exception {
                                ManifestList ret = new ManifestList();
                                for (Manifest remote : remoteList) {
                                    if (!localList.contains(remote)) {
                                        continue;
                                    }
                                    ret.add(remote);
                                }
                                return ret;
                            }
                        }
                )
                .flatMap(new Function<ManifestList, MaybeSource<? extends ManifestList>>() {
                    @Override
                    public MaybeSource<? extends ManifestList> apply(ManifestList manifestList) throws Exception {
                        if (manifestList.size() == 0) {
                            return Maybe.empty();
                        }
                        return Maybe.create(new MaybeOnSubscribe<ManifestList>() {
                            @Override
                            public void subscribe(MaybeEmitter<ManifestList> emitter) throws Exception {
                                downloadListener.onConfirm(manifestList, new Executor() {
                                    @Override
                                    public void execute() {
                                        downloadListener.onStart();
                                        emitter.onSuccess(manifestList);
                                    }

                                    @Override
                                    public void cancel() {
                                        downloadListener.onCancel();
                                        emitter.onComplete();
                                    }
                                });
                            }
                        }).subscribeOn(AndroidSchedulers.mainThread());
                    }
                })
                .flatMapCompletable(new Function<ManifestList, CompletableSource>() {
                    @Override
                    public CompletableSource apply(ManifestList manifestList) throws Exception {
                        return downloadFiles(manifestList, downloadListener);
                    }
                })
                .andThen(saveRemoteManifest())
                .subscribeOn(Schedulers.io())
                .observeOn(AndroidSchedulers.mainThread())
                .subscribe(new Action() {
                    @Override
                    public void run() throws Exception {
                        downloadListener.onDone();
                    }
                }, new Consumer<Throwable>() {
                    @Override
                    public void accept(Throwable throwable) throws Exception {
                        Log.i(TAG, "Download error.");
                        throwable.printStackTrace();
                        downloadListener.onError(throwable);
                    }
                });
    }

    public void clearCache() {
        File dir = new File(mConfig.getDocumentDir());
        deleteRecursive(dir);
    }
}
