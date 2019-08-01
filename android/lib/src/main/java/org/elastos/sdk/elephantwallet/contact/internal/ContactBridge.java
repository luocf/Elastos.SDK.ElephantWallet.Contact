package org.elastos.sdk.elephantwallet.contact.internal;

import android.util.Log;

import com.google.gson.Gson;
import com.google.gson.reflect.TypeToken;

import org.elastos.sdk.elephantwallet.contact.Contact;
import org.elastos.tools.crosspl.CrossBase;
import org.elastos.tools.crosspl.annotation.CrossClass;
import org.elastos.tools.crosspl.annotation.CrossInterface;

import java.util.ArrayList;
import java.util.List;

@CrossClass
public class ContactBridge extends CrossBase {
    public static final String TAG = "elastos";

    protected ContactBridge() {
        super(ContactBridge.class.getName(), 0);
    }

    public void finalize() {
        if(mListener != null) {
            mListener.unbind();
        }

        super.finalize();
    }


    public synchronized void setListener(Contact.Listener listener) {
        if(mListener != null) {
            mListener.unbind();
        }
        mListener = listener;

        mListener.bind();
        setListener(mListener);
    }

    public Contact.UserInfo getUserInfo() {
        assert(mListener != null);

        StringBuffer sbInfo = new StringBuffer();
        int ret = getHumanInfo("-user-info-", sbInfo);
        if(ret < 0) {
            Log.w(TAG, "Failed to get user info. ret=" + ret);
            return null;
        }


        Contact.UserInfo userInfo = new Contact.UserInfo();

        Contact.UserInfo.UserJson json = new Gson().fromJson(sbInfo.toString(), Contact.UserInfo.UserJson.class);
        ret  = userInfo.fromJson(json);
        if(ret < 0) {
            Log.w(TAG, "Failed to deserialize user info. ret=" + ret);
            return null;
        }

        return userInfo;
    }

    public List<Contact.FriendInfo> listFriendInfo() {
        assert(mListener != null);

        StringBuffer json = new StringBuffer();
        int ret = getFriendList(json);
        if(ret < 0) {
            Log.w(TAG, "Failed to list friend info. ret=" + ret);
            return null;
        }

        TypeToken<List<Contact.FriendInfo.FriendJson>> friendInfoListType = new TypeToken<List<Contact.FriendInfo.FriendJson>>(){};
        List<Contact.FriendInfo.FriendJson> listJson = new Gson().fromJson(json.toString(), friendInfoListType.getType());

        List<Contact.FriendInfo> list = new ArrayList<>();
        for(Contact.FriendInfo.FriendJson it: listJson) {
            Contact.FriendInfo friendInfo = new Contact.FriendInfo();
            ret  = friendInfo.fromJson(it);
            if(ret < 0) {
                Log.w(TAG, "Failed to deserialize friend info. ret=" + ret);
                return null;
            }

            list.add(friendInfo);
        }

        return list;
    }

    public List<String> listFriendCode() {
        List<String> friendCodeList = new ArrayList<String>();

        List<Contact.FriendInfo> friendList = listFriendInfo();
        if(friendList == null) {
            return friendCodeList;
        }

        for(Contact.FriendInfo it: friendList) {
            friendCodeList.add(it.humanCode);
        }

        return friendCodeList;
    }

    public ContactStatus getStatus(String humanCode) {
        int ret = getHumanStatus(humanCode);
        if(ret < 0) {
            return null;
        }

        return ContactStatus.valueOf(ret);
    }

    public Contact.Message makeMessage(ContactMessage.Type type, byte[] data, String cryptoAlgorithm) {
        Contact.Message msg = new Contact.Message(type, data, cryptoAlgorithm);
        return msg;
    }

    public Contact.Message makeTextMessage(String data, String cryptoAlgorithm) {
        Contact.Message msg = new Contact.Message(ContactMessage.Type.MsgText, data.getBytes(), cryptoAlgorithm);
        return msg;
    }


    public int sendMessage(String friendCode, ContactChannel channelType, Contact.Message message) {
        if(message == null) {
            return -1;
        }

        int ret = message.syncMessageToNative();
        if(ret < 0) {
            return ret;
        }

        ret = sendMessage(friendCode, channelType.id(), message);

        return ret;
    }

    @CrossInterface
    public native int start();

    @CrossInterface
    public native int addFriend(String friendCode, String summary);

    @CrossInterface
    public native int acceptFriend(String friendCode);

    @CrossInterface
    public native int syncInfoDownloadFromDidChain();

    @CrossInterface
    public native int syncInfoUploadToDidChain();

    @CrossInterface
    private native void setListener(CrossBase listener);

    @CrossInterface
    private native int getHumanInfo(String humanCode, StringBuffer info);

    @CrossInterface
    private native int getFriendList(StringBuffer info);

    @CrossInterface
    private native int getHumanStatus(String humanCode);

    @CrossInterface
    private native int sendMessage(String friendCode, int channelType, CrossBase message);

    private CrossBase mListener;

    static {
        System.loadLibrary("Elastos.SDK.Contact.Jni");
    }
}
