package org.elastos.sdk.elephantwallet.contact;

import android.util.Log;

import org.elastos.tools.crosspl.CrossBase;
import org.elastos.tools.crosspl.annotation.CrossClass;
import org.elastos.tools.crosspl.annotation.CrossInterface;

@CrossClass
abstract class ContactListener extends CrossBase {
    public class RequestArgs extends org.elastos.sdk.elephantwallet.contact.internal.RequestArgs {
        private RequestArgs(int type, String pubKey, byte[] data) {
            super(type, pubKey, data);
        }
    }

    public class EventArgs extends org.elastos.sdk.elephantwallet.contact.internal.EventArgs {
        public EventArgs(int type, String humanCode, int channelType, byte[] data) {
            super(type, humanCode, channelType, data);
        }
    }

    public ContactListener() {
        super(ContactListener.class.getName(), 0);
    }

    public abstract byte[] onRequest(RequestArgs request);
    public abstract void onEvent(EventArgs event);
    public abstract void onError(int errCode, String errStr);

    @CrossInterface
    private byte[] onRequest(int reqType, String pubKey, byte[] data) {
        Log.i(Contact.TAG, "ContactListener.onRequest()");

        RequestArgs args = new RequestArgs(reqType, pubKey, data);
        byte[] ret = onRequest(args);

        return ret;
    }

    @CrossInterface
    private void onEvent(int eventType, String humanCode, int channelType, byte[] data) {
        Log.i(Contact.TAG, "ContactListener.onRequest()");

        EventArgs args = new EventArgs(eventType, humanCode, channelType, data);
        onEvent(args);
        return;
    }

    static {
        System.loadLibrary("Elastos.SDK.Contact.Jni");
    }
}
