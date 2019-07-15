package org.elastos.sdk.elephantwallet.contact;

import android.util.Log;

import org.elastos.sdk.elephantwallet.contact.internal.HumanInfo;
import org.elastos.tools.crosspl.CrossBase;
import org.elastos.tools.crosspl.annotation.CrossClass;
import org.elastos.tools.crosspl.annotation.CrossInterface;

@CrossClass
public final class Contact extends CrossBase {
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

    public final class UserInfo extends org.elastos.sdk.elephantwallet.contact.internal.UserInfo {
    } // class UserInfo

    public synchronized void setListener(Listener listener) {
        if(mListener != null) {
            mListener.unbind();
        }
        mListener = listener;

        mListener.bind();
        setListener(mListener);
    }

    public UserInfo getUserInfo() {
        assert(mListener != null);

        StringBuffer json = new StringBuffer();
        int ret = getHumanInfo("-user-info-", json);
        if(ret < 0) {
            Log.w(TAG, "Failed to get user info. ret=" + ret);
            return null;
        }

        UserInfo userInfo = new UserInfo();

        ret  = userInfo.deserialize(json.toString(), ContactStatus.Invalid);
        if(ret < 0) {
            Log.w(TAG, "Failed to deserialize user info. ret=" + ret);
            return null;
        }

        userInfo.status = getStatus(userInfo.did);

        return userInfo;
    }

    public HumanInfo getFriendInfo(String friendCode) {
        return null;
    }

    public ContactStatus getStatus(String humanCode) {
        int ret = getHumanStatus(humanCode);
        if(ret < 0) {
            return null;
        }

        return ContactStatus.valueOf(ret);
    }

    @CrossInterface
    public native int start();

    @CrossInterface
    public native int addFriend(String friendCode, String summary);

    @CrossInterface
    private native void setListener(CrossBase listener);

    @CrossInterface
    private native int getHumanInfo(String humanCode, StringBuffer info);

    @CrossInterface
    private native int getHumanStatus(String humanCode);

    private Contact() {
        super(Contact.class.getName(), 0);
    }

    public void finalize() {
        if(mListener != null) {
            mListener.unbind();
        }

        super.finalize();
    }

    private CrossBase mListener;

    static {
        System.loadLibrary("Elastos.SDK.Contact.Jni");
    }
}
