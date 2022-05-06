package com.snakat.proxy;

import com.snakat.proxy.data.ManifestList;

import io.reactivex.Maybe;
import io.reactivex.Single;
import okhttp3.ResponseBody;
import retrofit2.http.GET;
import retrofit2.http.Path;
import retrofit2.http.Url;

interface ManifestService {
    @GET("{path}")
    Maybe<ManifestList> getManifest(@Path("path") String path);

    @GET("{path}")
    Single<ResponseBody> downloadFile(@Path(("path")) String path);
}
