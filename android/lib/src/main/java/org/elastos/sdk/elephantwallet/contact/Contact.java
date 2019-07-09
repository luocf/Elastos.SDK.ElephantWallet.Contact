package org.elastos.sdk.elephantwallet.contact;

import android.util.Log;

import org.elastos.tools.crosspl.CrossBase;
import org.elastos.tools.crosspl.annotation.CrossClass;
import org.elastos.tools.crosspl.annotation.CrossInterface;

@CrossClass
public final class Contact extends CrossBase {
    static final String TAG = "elastos";

    public static final class Factory extends ContactFactory {
        public static Contact Create() {
            return new Contact();
        }

        private Factory() { }
    } // class Factory

    public abstract static class Listener extends ContactListener {
    } // class Factory

    public void setListener(Listener listener) {
        setListener((CrossBase)listener);
    }

    @CrossInterface
    public native int start();

    @CrossInterface
    private native void setListener(CrossBase listener);

    private Contact() {
        super(Contact.class.getName(), 0);
    }

    static {
        System.loadLibrary("Elastos.SDK.Contact.Jni");
    }
}
