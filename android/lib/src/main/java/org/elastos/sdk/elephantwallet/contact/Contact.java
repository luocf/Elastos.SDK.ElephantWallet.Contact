package org.elastos.sdk.elephantwallet.contact;

import android.util.Log;

import org.elastos.tools.crosspl.CrossBase;
import org.elastos.tools.crosspl.annotation.CrossClass;
import org.elastos.tools.crosspl.annotation.CrossInterface;

@CrossClass
public final class Contact extends CrossBase {
    static final String TAG = "elastos";

    @CrossClass
    public static final class Factory extends CrossBase {
        @CrossInterface
        public static native void SetLogLevel(int level);

        @CrossInterface
        public static native int SetLocalDataDir(String dir);

        public static Contact Create() {
            return new Contact();
        }

        private Factory() { }

        static {
            System.loadLibrary("Elastos.SDK.Contact.Jni");
        }
    } // class Factory

//    @CrossInterface
//    public native void setListener();

    private Contact() { }
}
