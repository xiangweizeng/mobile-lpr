package com.mobilelpr.demo;

import android.app.Application;
import android.content.Context;
import android.util.Log;

import androidx.multidex.MultiDex;

import com.zxy.recovery.callback.RecoveryCallback;
import com.zxy.recovery.core.Recovery;


public class NcnnApp extends Application {

    @Override
    public void onCreate() {
        super.onCreate();
        initRecovery();
    }

    @Override
    protected void attachBaseContext(Context base) {
        super.attachBaseContext(base);
        MultiDex.install(base);
    }

    private void initRecovery() {
        Recovery.getInstance()
                .debug(BuildConfig.DEBUG)
                .recoverInBackground(true)
                .recoverStack(true)
                .mainPage(MainActivity.class)
                .recoverEnabled(true)
                .callback(new MyCrashCallback())
                .silent(false, Recovery.SilentMode.RECOVER_ACTIVITY_STACK)
                .init(this);
        AppCrashHandler.register();
    }

    static final class MyCrashCallback implements RecoveryCallback {
        @Override
        public void stackTrace(String exceptionMessage) {
            Log.e("mobilelpr", "exceptionMessage:" + exceptionMessage);
        }

        @Override
        public void cause(String cause) {
            Log.e("mobilelpr", "cause:" + cause);
        }

        @Override
        public void exception(String exceptionType, String throwClassName, String throwMethodName, int throwLineNumber) {
            Log.e("mobilelpr", "exceptionClassName:" + exceptionType);
            Log.e("mobilelpr", "throwClassName:" + throwClassName);
            Log.e("mobilelpr", "throwMethodName:" + throwMethodName);
            Log.e("mobilelpr", "throwLineNumber:" + throwLineNumber);
        }

        @Override
        public void throwable(Throwable throwable) {

        }
    }

}
