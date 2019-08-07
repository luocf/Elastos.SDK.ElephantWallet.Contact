package org.elastos.sdk.elephantwallet.contact;

import org.elastos.sdk.elephantwallet.contact.internal.ContactBridge;
import org.elastos.sdk.elephantwallet.contact.internal.ContactDebug;
import org.elastos.sdk.elephantwallet.contact.internal.ContactFactory;
import org.elastos.sdk.elephantwallet.contact.internal.ContactListener;

public final class Contact extends ContactBridge {
    public static final String TAG = "elastos";

    public static final class Factory extends ContactFactory {
        public static Contact Create() {
            return new Contact();
        }

        public static void SetDeviceId(String devId) {
            ContactFactory.SetDeviceId(devId);
            UserInfo.setCurrDevId(devId);
        }

        private Factory() { }
    } // class Factory

    public abstract static class Listener extends ContactListener {
    } // class Listener

    public static final class UserInfo extends org.elastos.sdk.elephantwallet.contact.internal.UserInfo {
    } // class UserInfo

    public static final class FriendInfo extends org.elastos.sdk.elephantwallet.contact.internal.FriendInfo {
    } // class FriendInfo

    public static final class Message extends org.elastos.sdk.elephantwallet.contact.internal.ContactMessage {
        public Message(Type type, byte[] data, String cryptoAlgorithm) {
            super(type, data, cryptoAlgorithm);
        }
    } // class Message

    public abstract static class Debug extends ContactDebug {
    } // class Listener

    private Contact() {
    }
}
