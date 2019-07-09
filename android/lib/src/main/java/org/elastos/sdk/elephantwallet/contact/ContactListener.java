package org.elastos.sdk.elephantwallet.contact;

import android.telecom.Call;
import android.util.Log;

import org.elastos.tools.crosspl.CrossBase;
import org.elastos.tools.crosspl.annotation.CrossClass;
import org.elastos.tools.crosspl.annotation.CrossInterface;

@CrossClass
abstract class ContactListener extends CrossBase {
    public interface CallbackType {
        static final int Error = -10000;
        static final int Request = 20000;
        static final int Event = 30000;

        // request
        static final int PublicKey = 1;
        static final int EncryptData = 2;
        static final int DecryptData = 3;
        static final int DidPropAppId = 4;
        static final int DidAgentAuthHeader = 5;
        static final int SignData = 6;


        static final int StatusChanged = 10;
        static final int ReceivedMessage = 11;
        static final int SentMessage = 12;
        static final int FriendRequest = 13;
        static final int FriendStatusChanged = 14;

        static int GetKind(int type) {
            if(type < 0) {
                return Error;
            } else if (type > Request && type < Event ) {
                return Request;
            } else if (type > Event ) {
                return Event;
            } else {
                throw new RuntimeException("Unknown ContactListener.CallbackType: " + type);
            }
        }

        static int UnpackType(int type) {
            int kind = GetKind(type);
            return (type - kind);
        }
    }

    public ContactListener() {
        super(ContactListener.class.getName(), 0);
    }

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

        byte[] ret = null;

        int kind = CallbackType.GetKind(type);
        switch (kind) {
            case CallbackType.Error:
                int code = CallbackType.UnpackType(type);
                onError(code, null);
                break;
            case CallbackType.Request:
                onRequest(null);
                break;
            case CallbackType.Event:
                onEvent(null);
                break;
        }

        return ret;
    }

    static {
        System.loadLibrary("Elastos.SDK.Contact.Jni");
    }
}
