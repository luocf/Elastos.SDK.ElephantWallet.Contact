package org.elastos.sdk.elephantwallet.contact;

import android.util.Log;

import org.elastos.tools.crosspl.CrossBase;
import org.elastos.tools.crosspl.annotation.CrossClass;
import org.elastos.tools.crosspl.annotation.CrossInterface;

@CrossClass
class ContactFactory extends CrossBase {
    @CrossInterface
    public static native void SetLogLevel(int level);

    @CrossInterface
    public static native int SetLocalDataDir(String dir);

    protected ContactFactory() { }

    static {
        System.loadLibrary("Elastos.SDK.Contact.Jni");
    }
} // class Factory
