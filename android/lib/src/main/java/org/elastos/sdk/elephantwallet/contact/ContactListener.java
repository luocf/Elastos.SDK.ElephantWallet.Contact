package org.elastos.sdk.elephantwallet.contact;

import android.util.Log;

import org.elastos.tools.crosspl.CrossBase;
import org.elastos.tools.crosspl.annotation.CrossClass;
import org.elastos.tools.crosspl.annotation.CrossInterface;

@CrossClass
abstract class ContactListener  extends CrossBase {
    public abstract void onRequest(Request request);
    public abstract void onEvent(Event event);
    public abstract void onError(int errCode, String errStr);

    public class Request {

    }

    public class Event {

    }

    public static <T extends Request> T Cast(Request request) {
        T ret = null;
        if (request.getClass() == ret.getClass()) {
            return (T) request;
        }

        throw new RuntimeException("Failed to cast request to " + ret.getClass());
    }

    public static <T extends Event> T Cast(Event request) {
        T ret = null;
        if (request.getClass() == ret.getClass()) {
            return (T) request;
        }

        throw new RuntimeException("Failed to cast event to " + ret.getClass());
    }


    @CrossInterface
    private byte[] onCallback(int type, byte[] args) {
        Log.i(Contact.TAG, "ContactListener.onCallback()");

        return null;
    }

    static {
        System.loadLibrary("Elastos.SDK.Contact.Jni");
    }
}
